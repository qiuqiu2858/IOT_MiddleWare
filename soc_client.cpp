#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define MAXLINE 4096
  
  
int main(int argc, char** argv)  
{  
    int    sockfd, n,rec_len;  
    char    recvline[4096], sendline[4096];  
    char    buf[MAXLINE];  
    struct sockaddr_in    servaddr;   
  
    if( argc != 2){  
    printf("usage: ./client <ipaddress>\n");  
    exit(0);  
    }  
  
  
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
      printf("create SOCKET ERROR: %s(ERRNO: %d)\n", strerror(errno),errno);  
      exit(0);  
    }  
  
  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(8000);
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){  
    printf("inet_pton error for %s\n",argv[1]);  
    exit(0);  
    }  
  
  
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){  
    printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
    exit(0);  
    }  
  
    const char* q = "quit\n";
    //fgets(sendline,4096,stdin);
    //printf("%s",sendline);
    if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {
        perror("recv error");  
        exit(1);  
    }  
    buf[rec_len]  = '\0';  
    printf("Received : %s ",buf);
    while( strcmp(sendline,q) != 0 ) {
      printf("sending msg to server!\n");  
      if( send(sockfd, sendline, strlen(sendline), 0) < 0)  
      {  
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);  
        exit(0);  
      }  
      if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {
        perror("recv error");  
        exit(1);  
      }  
      buf[rec_len]  = '\0';  
      //printf("Received : %s ",buf);
      memset(sendline,0,sizeof(sendline));
      fgets(sendline, 4096, stdin);
      //close(sockfd);  
      //exit(0);
      //fgets(sendline, 4096, stdin);
    }
    printf("getting here!!");
    close(sockfd);
    exit(0);
}  
