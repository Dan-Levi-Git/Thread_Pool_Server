#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "queue.h"

#define SERVER_PORT     (9898U)
#define SERVER_BACKLOG   (100U)
#define THREAD_POOL_SIZE (4U)
#define SOCKTERROR       (-1)



pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t threadlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
queue_node* queue_pool;
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;


/* Check function for errors in socket. */

int check(int exp, const char *msg)
{
    if (exp == SOCKTERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}

/* Function that handel the thread dequeue from the pool, 
   read from file and send it to the client */

void *handle_connction (int *p_client)
{
    int n;
    int stat;
    int client_psocket = *(p_client);
    char buffer[256];
    FILE *fp;
    fp = fopen("test1.txt", "r");
    fread(buffer, sizeof(char), sizeof(buffer), fp);
    write(client_psocket, buffer, sizeof(buffer));
    printf("File send successfuly \n");
    fclose(fp);
    while(1)
    {
        bzero(buffer ,sizeof(buffer));
        read(client_psocket,buffer,sizeof(buffer));
        printf("Client: %s \n",buffer);
        stat = strncmp("Bye", buffer, strlen("Bye"));
        if(stat == 0)
        {
            printf("Connection close \n");
            break;
        }
    }
    close(client_psocket);
}

/* Function that mainten the pool, dequeue from the link tasks
  and move them to free thread that serv the pool. */

void *thread_function(void *arg)
{
    while(1)
    {
        int* pclint;
        pthread_mutex_lock(&threadlock);
        pclint = Dequeue(queue_pool);
        if (pclint == NULL)
        {
            pthread_cond_wait(&thread_cond, &threadlock);
            pclint = Dequeue(queue_pool);
        }

        pthread_mutex_unlock(&threadlock);

        if(pclint != NULL)
        {
            handle_connction(pclint);
        }
    }

}

int main(int argc, char *argv[])
{
    int client_socket , server_socket;
    socklen_t addr_size;
    SA_IN server_addr , clinet_addr;
    int i;

    /* Initialize queue of thread pool. */
    queue_pool = init_queue();

    /* Creaet a set of threads for that serve as the active threads of the rhread pool. */
    for(i = 0; i < THREAD_POOL_SIZE; i++)
    {
        check(pthread_create(&thread_pool[i], NULL, &thread_function, NULL), "Failed to create thread)");
    }

    /* Creaet a socket of TCP. */
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0), "Failed to create a socket \n");

    /* Initialize the address struct. */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Failed bind to a client... \n");

    check(listen(server_socket, SERVER_BACKLOG), "Failed listen to client..\n");

    while(1)
    {
        printf("waiting for connection...\n");
        addr_size = sizeof(SA_IN);
        check(client_socket = accept(server_socket, (SA *)&clinet_addr, &addr_size),"Failed to acccept connection");
        printf("connected successfuly! \n");

        /* Enter task (thread) to the pool. */
        int* psockt = (int*)malloc(sizeof(int));
        *psockt = client_socket;
        pthread_mutex_lock(&threadlock);
        enqueu(psockt , queue_pool);
        pthread_mutex_unlock(&threadlock);
        pthread_cond_signal(&thread_cond);

    }

    /* Join the thread that finish the job. */
    for (i = 1; i < THREAD_POOL_SIZE; i++)
    {
        pthread_join(thread_pool[i], NULL);
    }
    
    pthread_mutex_destroy(&threadlock);
    pthread_cond_destroy(&thread_cond);
    close(server_socket);

    return 0;
}

