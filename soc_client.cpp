#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string>

#define MAXLINE 4096

using namespace std;
  
  
int main(int argc, char** argv)  
{  
    int sockfd, n,rec_len;  
    char recvline[4096], sendline[4096];  
    char buf[MAXLINE];  
    struct sockaddr_in servaddr;
    int server_or_client; // 0 stands for server, 1 stands for client
    int local_port;
    struct sockaddr_in client_addr;
    int fd_server;
  
    if( argc != 4){  
    printf("usage: ./client <ipaddress> <flag> <port>\n");  
    exit(0);  
    }

    server_or_client = atoi(argv[2]);
    local_port = atoi(argv[3]);

    printf("server or client: %d, local port: %d\n", server_or_client,local_port);
    
    // create a new socket
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
      printf("create SOCKET ERROR: %s(ERRNO: %d)\n", strerror(errno),errno);  
      exit(0);  
    }

    // set the socket so that the port can be reused
    int opt_val = 1;
    int opt_len = sizeof(int);
    if( setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt_val,opt_len) < 0 ) {
      printf("set SOCKET REUSE ERROR: %s(ERRNO: %d)\n", strerror(errno),errno);
      exit(1);
    }

    // bind local client ip & port if the client is a server
    if( server_or_client == 0 ) {
      memset(&client_addr,0,sizeof(client_addr));
      client_addr.sin_family = AF_INET;
      client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      client_addr.sin_port = htons(local_port);
      if( bind(sockfd,(struct sockaddr*)&client_addr, sizeof(client_addr)) < 0 )
      {
        perror("bind failed");
        exit(1);
      }
    }

    // set server ip & port
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(8000);
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){  
      printf("inet_pton error for %s\n",argv[1]);  
      exit(0);  
    }  
  
    // connect the remote server
    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){  
      printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
      exit(0);  
    }  
    
    // get ip and port if the client is a server
    if( server_or_client == 0 ) {
      memset(&client_addr,0,sizeof(client_addr));
      int addr_len = sizeof(client_addr);
      if(getsockname(sockfd, (struct sockaddr*)&client_addr,(socklen_t*)&addr_len) < 0) 
      {
	printf("get sockname");
	exit(1);
      }
      printf("the port is %d\n", htons(client_addr.sin_port));
      fd_server = socket(AF_INET,SOCK_STREAM,0);
      if( fd_server < 0 ) {
	printf("init socket failed\n");
	exit(1);
      }
      if( setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR,&opt_val,opt_len)<0 ) {
	printf("setsockopt error\n");
	exit(1);
      }
      memset(&servaddr,0,sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = client_addr.sin_port;
      servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      if(bind(fd_server,(struct sockaddr*)&servaddr,sizeof(servaddr)) <0) {
        perror("bind");
	exit(1);
      }
      if( listen(fd_server,10) < 0) {
        perror("listen");
      }
      printf("listen success\n");
    }

    const char* q = "quit\n";
    if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {
        perror("recv error");  
        exit(1);  
    }  
    //buf[rec_len]  = '\0';
    sockaddr_in rec_addr;
    memcpy(&rec_addr,buf,sizeof(rec_addr));
    printf("Received : the remote ip is %s ,the remote port is %d\n",inet_ntoa(rec_addr.sin_addr),htons(rec_addr.sin_port));
    

    // is a server, handle with the clients' request
    if( server_or_client == 0 ) {
      printf("server is listening!\n");
      sockaddr_in remote_client_addr;
      socklen_t remote_client_addr_len;
      memset(&remote_client_addr,0,sizeof(remote_client_addr));
      remote_client_addr_len = sizeof(remote_client_addr);
      int client_fd = accept(fd_server,(struct sockaddr*)&remote_client_addr,&remote_client_addr_len);
      string text = "hello, i connect u";
      if( send(client_fd,text.c_str(),text.length(),0) == -1 ) {
	perror("send error");
      }
      printf("server sended data already\n");
    }
    
    // is a client, connect the new socket
    if( server_or_client == 1 ) {
      printf("client is requesting\n");
      int remote_sock_fd;
      if( (remote_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
	printf("create SOCKET ERROR: %s(ERRNO: %d)\n", strerror(errno),errno);  
	exit(0);  
      }
 
      if( connect(remote_sock_fd, (struct sockaddr*)&rec_addr, sizeof(rec_addr)) < 0){  
	printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
	exit(0);  
      }
      memset(buf,0,sizeof(buf));
      if((rec_len = recv(remote_sock_fd, buf, MAXLINE,0)) == -1) {
        perror("recv error");  
        exit(1);
      }
      printf("Received data from Server 1: %s\n",buf);
      
      
    }
      



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
