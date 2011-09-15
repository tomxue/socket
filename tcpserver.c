/*
 * =====================================================================================
 *
 *       Filename:  1.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/14/11 08:31:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tom Xue (), tom.xue@nokia.com
 *        Company:  Nokia
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
int main()
{
int sockfd,new_fd;
struct sockaddr_in my_addr;
struct sockaddr_in their_addr;
int sin_size;

//建立TCP套接口
//AF_INET: Internet IP Protocol
//SOCK_STREAM: Sequenced, reliable, connection-based byte streams
//0: IPPROTO_IP = 0, Dummy protocol for TCP
if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) 
{
	printf("create socket error");
	perror("socket");
	exit(1);
}

////初始化sockaddr_in结构体（地址和通道），并绑定2323端口
my_addr.sin_family = AF_INET;
//host byte order to net
my_addr.sin_port = htons(2323); 
//INADDR_ANY: Address to accept any incoming messages
my_addr.sin_addr.s_addr = INADDR_ANY;
//#define sin_zero __pad
bzero(&(my_addr.sin_zero),8);

////绑定套接口
if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
{
	perror("bind socket error");
	exit(1);
}

////创建监听套接口
//N connection requests will be queued before further requests are refused.
if(listen(sockfd,10)==-1)
{
	perror("listen");
	exit(1);
}

////等待连接
while(1)
{
	sin_size = sizeof(struct sockaddr); //either sockaddr or sockaddr_in can work normally

	printf("server is run.\n");

	////如果建立连接，将产生一个全新的套接字
	if((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size))==-1)
	{
		perror("accept");
		exit(1);
	}
	printf("accept success.\n");
	
	////生成一个子进程来完成和客户端的会话，父进程继续监听
	//fork: Return -1 for errors, 0 to the new process
	if(!fork())
	{
		printf("create new thred success.\n");

		////读取客户端发来的信息
		int numbytes;
		char buff[256];
		memset(buff,0,256);

	if((numbytes = recv(new_fd,buff,sizeof(buff),0))==-1)
	{
		perror("recv");
		exit(1);
	}
	printf("%s\n",buff);
	
	////将从客户端接收到的信息再发回客户端
	if(send(new_fd,buff,strlen(buff),0)==-1)
		perror("send");
	
	close(new_fd);
	exit(0);
	}
	close(new_fd);
}

close(sockfd);
}
