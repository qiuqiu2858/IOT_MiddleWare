#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <iostream>

#define default_port 8000
#define MAXLINE 4096

using namespace std;

string Int2String(int value);

int main(int argc, char** argv)
{

  int socket_fd, connect_fd_1,connect_fd_2;
  struct sockaddr_in servaddr;
  struct sockaddr_in clientaddr_1;
  struct sockaddr_in clientaddr_2;
  socklen_t clientaddrlen_1;
  socklen_t clientaddrlen_2;
  int cnt = 0;
  char buff[4096];
  int n;

  if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
    printf("create socket error: %s(error: %d)\n", strerror(errno),errno);
    exit(0);
  }

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(default_port);
  clientaddrlen_1 = sizeof(clientaddr_1);
  clientaddrlen_2 = sizeof(clientaddr_2);

  if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1 ) {
    printf("bind socket error: %s(errno: %d)\n", strerror(errno),errno);
    exit(0);
  }

  if( listen(socket_fd,10) == -1 ) {
    printf("listen socket error: %s(error: %d)\n",strerror(errno),errno);
    exit(0);
  }

  while(1){
    if( cnt == 0 ) {
      connect_fd_1 = accept(socket_fd, (struct sockaddr*)&clientaddr_1, &clientaddrlen_1);
      cnt++;
    }
    if( cnt == 1 ) {
      connect_fd_2 = accept(socket_fd, (struct sockaddr*)&clientaddr_2, &clientaddrlen_2);
      char* res1 = new char[sizeof(clientaddr_1)];      
      char* res2 = new char[sizeof(clientaddr_2)];
      memset(res1,0,sizeof(res1));
      memset(res2,0,sizeof(res2));
      memcpy(res1,&clientaddr_1,sizeof(res1));
      memcpy(res2,&clientaddr_2,sizeof(res2));
      

      if( send(connect_fd_1, res2,sizeof(res2),0) == -1 )
	perror("send error");
      if( send(connect_fd_2, res1,sizeof(res1),0) == -1 )
        perror("send error");
    }
    //if( connect_fd == -1 ) {
    //  continue;
    //}
       
    //char replytext[4096];
    
    //n = recv(connect_fd, buff, MAXLINE, 0);
    //strcpy(replytext,text);
    //strcat(replytext,buff);
    //replytext[strlen(replytext)] = '\0';
    //if( send(connect_fd, replytext,strlen(replytext),0) == -1 )
    //  perror("send error");
    //buff[n] = '\0';
    //printf("recv msg from client: %s", buff);
    //printf("the client ip address is %s, the port is %d\n", inet_ntoa(clientaddr.sin_addr),clientaddr.sin_port);
    //memset(buff,0,sizeof(buff));
    //memset(replytext,0,sizeof(replytext));
  
  }
  close(socket_fd);

}

string Int2String(int value)
{
  string res;
  stringstream ss;
  ss<<value;
  return ss.str();
}
