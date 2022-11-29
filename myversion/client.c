#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()
void echo(int sockfd)
{
    char buff[1000];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf(" ---> : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("server\n: %s ", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Клиент Отключен...\n");
            break;
        }
    }
}
int main(){
int ls;
ls = socket(AF_INET,SOCK_STREAM,0);	
struct sockaddr_in servaddr;
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(5000);
servaddr.sin_addr.s_addr = INADDR_ANY;
if (0 != connect(ls, (struct sockaddr *) &servaddr,sizeof(servaddr)))
{
	printf("connection with the server failed...\n");
    exit(0);
}

else{
    printf("connected to the server..\n");
}
echo(ls);
close(ls);
}
