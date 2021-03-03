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

#define SERVER_PORT 9898
#define THREAD_POOL_SIZE 4


pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t threadlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
queue_node* queue_pool;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;


/*function that handel in the thread from the pool
  send for client a text and client print is content
  and enter for chat mode with server */

void *handle_connction (int *p_client)
{
    int n;
    int stat;
    int client_psocket = *(p_client);
    char buffer[256];
    FILE *fp;
    fp = fopen("test1.txt", "r");
    fread(buffer, sizeof(char), sizeof(buffer), fp);
    write(client_psocket, buffer, 256);
    printf("file is send \n");
    fclose(fp);
    while(1)
    {
        bzero(buffer ,256);
        read(client_psocket,buffer,256);
        printf("the client say: %s \n",buffer);
        stat = strncmp("Bey", buffer, 3);
        if(stat == 0)
        {
            break;
        }
    }
    close(client_psocket);
}

/*function that mainten the pool, dequeue from the link tasks
  and move them to free thread that serv the pool*/

void *thread_function(void *arg)
{
    while(1)
    {
        int* pclint;
        pthread_mutex_lock(&threadlock);

        if ((pclint = Dequeue(queue_pool)) == NULL)
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
    int clinet_socket , server_socket;
    socklen_t addr_size;
    SA_IN server_addr , clinet_addr;
    int i;

    //init queue of thread pool
    queue_pool = init_queue();

    //creaet a set of threads that serve the task from the pool
    for(i = 1; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&thread_pool[i], NULL, &thread_function, NULL);
    }

    //creaet a socket of TCP 
    server_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(server_socket < 0)
    {
        printf("error creaet socket \n");
    }

    //config the server 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket , (SA*) &server_addr , sizeof(server_addr)) < 0)
    {
        printf("ERROR bind a client... \n");
    }

    if(listen(server_socket, 100) < 0 )
    {
        printf("ERROR listen..\n");
    }

    while(1)
    {
        printf("waiting for connection...\n");
        addr_size = sizeof(SA_IN);
        clinet_socket = accept(server_socket, (SA *)&clinet_addr, &addr_size);
        printf("connected!\n");

        //enter task (thread) to the pool  
        int* psockt = (int*)malloc(sizeof(int));
        *psockt = clinet_socket;
        pthread_mutex_lock(&threadlock);
        enqueu(psockt , queue_pool);
        pthread_cond_signal(&thread_cond);
        pthread_mutex_unlock(&threadlock);

    }

    //join the thread that finish the job
    for (i = 1; i < THREAD_POOL_SIZE; i++)
    {
        pthread_join(thread_pool[i], NULL);
    }
    
    pthread_mutex_destroy(&threadlock);
    pthread_cond_destroy(&thread_cond);
    close(server_socket);

    return 0;
}
