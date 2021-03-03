#include <stdio.h>
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
#include <ctype.h>


#define SOCKTERROR -1

/* Check function for errors in socket. */
int check(int exp , const char* msg)
{
    if(exp == SOCKTERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}


int main(int argc , char *argv[])
{
    int sockfd ,portnum, n, i;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[256];

     if (argc < 3)
     {
         printf("Please provied legal host \n");
         exit(1);
     }

    /* Geting thr port num from shell. */
    portnum = atoi(argv[2]);

    check(sockfd = socket(AF_INET, SOCK_STREAM , 0),"Failed creaet socket");
    
    /* Gating the host name by shell. */
    server = gethostbyname(argv[1]);


    if(server == NULL)
    {
        printf("Server name not legal try again ... \n");
        exit(1);
    }

    /* Initialize the address struct. */
    bzero((char*)&serveraddr , sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(portnum);
    bcopy((char*)server->h_addr,(char*)&serveraddr.sin_addr.s_addr, server->h_length);

    check(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)), "Failed connecting to sockt \n");

    /* Read from file creaeted it  on the client machine ,and print is content. */
    bzero(buffer, 256);
    FILE *fd;
    read(sockfd,buffer,256);
    int stat;
    fd = fopen("test1_recive.txt", "a");
    fwrite(buffer, sizeof(char), strlen(buffer), fd);
    printf("Receive file successfuly. \nThe content is: %s \n", buffer);
    fclose(fd);
    printf("Now you can chat with the server:\n");

     while (1)
    {
        bzero(buffer ,256);
        fgets(buffer,sizeof(buffer),stdin);
        write(sockfd, buffer,strlen(buffer));
        stat = strncmp("Bey", buffer, 3);
        if (stat == 0)
        {
            break;
        }
    }
