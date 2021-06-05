//  
// file_client.c   socket传输文件的client端示例程序   
//
#include<netinet/in.h>                         // for sockaddr_in  
#include<sys/types.h>                          // for socket  
#include<sys/socket.h>                         // for socket  
#include<stdio.h>                              // for printf  
#include<stdlib.h>                             // for exit  
#include<string.h>                             // for bzero  
#include<arpa/inet.h>  
#include<unistd.h>
 
#define SERV_PORT 	17980                   //服务器端口
#define SERV_ADDR 	"192.168.44.129"         //服务器ip
char recv_buf[4096];        //定义接收缓存区
char send_buf[4096];        //定义发送缓存区

int main(int argc, char **argv)  
{  
	int ret = -1;
	int sockfd = -1;  //定义网络文件描述符
	FILE *fp = NULL;  //定义文件操作指针
	
	struct sockaddr_in servaddr={0};  //服务器sockaddr_in定义成ipv4类型的服务器ip结构体（ipv6是sockaddr_inv6）

	//1.首先使用socket函数创建网络文件描述符（类似于文件IO中的open函数）
    //函数原型：int socket(int domain, int type, int protocol);   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);   //ipv4,TCP,系统自动选择protocol
	if(sockfd < 0)
	{
		printf("创建socket文件描述符失败\n");
		_exit(-1);
	}
	printf("sockfd=%d\n",sockfd);
	//2.使用connect函数连接服务器
	//函数原型：int connect(int socket, const struct sockaddr *address,socklen_t address_len);
	servaddr.sin_family = AF_INET;             			//定义servaddr的domain地址族为ipv4
	servaddr.sin_port = htons(SERV_PORT);      			//定义servaddr的portnum为SERV_PORT(8010),host to net short
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);  	//定义servaddr的address为SERV_ADDR(192.168.1.23)  person----->u32

    ret = connect(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr));
	if(ret < 0)
	{
		printf("客户端connect服务器失败\n");
		_exit(-1);
	}
	printf("客户端connect服务器成功\n");
	
	//下面客户端和服务器互相收发
	while(1)
	{
		//3.使用send函数发生数据
		printf("请输入要发送给服务器的内容: \n");
		scanf("%s", send_buf);  
		if(!strncmp(send_buf,"+++",3))break;	//输入+++客户端断开连接
		fp = fopen(send_buf, "w");  
		if (fp == NULL)  
		{  
			printf("File:\t%s Can Not Open To Write!\n", send_buf);  
			_exit(-1);  
		}  
		printf("File:\t%s Open Success,Waitting To Write...\n", send_buf); 
		
		ret = send(sockfd, send_buf, strlen(send_buf), 0);
		printf("send buffer:%s,sned len:%d\n",send_buf,ret);
		
		//4.使用recv函数接收来自服务端的消息
		ret = recv(sockfd, recv_buf, sizeof(recv_buf), 0);   
		if(ret < 1)
		{
			printf("服务器断开了连接\n");
			break;
		}
		printf("收到服务器发送的数据:recv buffer:\n%s\nrecv len:%d\n",recv_buf,ret);
		printf("将接收到的数据写入文件中:\n");
		//调用fwrite函数将recv_buf缓存中的数据写入文件中
		int write_length = fwrite(recv_buf, sizeof(char), ret, fp);  
        if (write_length < ret)  
        {  
            printf("文件写入失败!\n");  
            break;  
        }  
		printf("Recieve File:\t %s From Server[%s] 接收成功!\n", send_buf, SERV_ADDR);  
		memset(send_buf,0,sizeof(send_buf));   //清空接收缓存区
		memset(recv_buf,0,sizeof(recv_buf));   //清空接收缓存区
		fclose(fp);  
	}
	printf("关闭连接并退出\n");
	close(sockfd);     //关闭socket文件描述符
	return 0;
}  