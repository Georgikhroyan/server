#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
void echo(int connfd)
{
    char buff[100];
    int n;
    for (;;) {
        bzero(buff, 100);
        read(connfd, buff, sizeof(buff));
        write(connfd, buff, sizeof(buff));
           if (strncmp("exit", buff, 4) == 0) {
            printf("Сервер Отключен...\n");
            break;
        }
    }
}

int main(){
  int ls;
  int len;
  int connfd;
  ls = socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(ls, (struct sockaddr *) &addr, sizeof(addr));
  if ((listen(ls, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(addr);
   
    // Accept the data packet from client and verification
    connfd = accept(ls, (struct sockaddr *) &addr, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
    echo(connfd);
    close(connfd);
}