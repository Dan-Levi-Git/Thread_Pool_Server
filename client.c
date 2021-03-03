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

#define SERVER_PORT 9696

int main(int argc , char *argv[])
{
    int sockfd ,portnum, n, i;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[256];
     if (argc < 3)
     {
         printf("error plaes provied legal host\n");
     }

    portnum = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM , 0);
    if( sockfd < 0)
    {
        printf("faield open socket \n");
    }

    server = gethostbyname(argv[1]);

    if(server == NULL)
    {
        printf("ERROR: server name... \n");

    }
    bzero((char*)&serveraddr , sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(portnum);
    bcopy((char*)server->h_addr,(char*)&serveraddr.sin_addr.s_addr, server->h_length);

    if(connect(sockfd, (struct sockaddr*)& serveraddr, sizeof(serveraddr)) < 0)
    {
        printf("error connecting to sockt \n");
    }

    bzero(buffer , 256);
    FILE *fd;
    read(sockfd,buffer,256);
    int stat;
    fd = fopen("test1_recive.txt", "a");
    fwrite(buffer, sizeof(char), strlen(buffer), fd);
    printf("receive file the content is: %s \n",buffer);
    fclose(fd);
    while(1)
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
    close(sockfd);
    return 0;
}