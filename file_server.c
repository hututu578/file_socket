//  
// file_server.c   socket传输文件的server端示例程序   
//
#include<netinet/in.h>   
#include<sys/types.h>   
#include<sys/socket.h>   
#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>   
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 17980						//定义服务器端口号
#define SERVER "192.168.44.129"			//定义服务器的IP地址

#define LENGTH_OF_LISTEN_QUEUE     20  
#define BUFFER_SIZE                4096  
#define FILE_NAME_MAX_SIZE         512  
  
int main(int argc, char **argv)  
{  
	FILE *fp = NULL;
    // set socket's address information   
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口  
    struct sockaddr_in   server_addr;  
    bzero(&server_addr, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = inet_addr(SERVER);  
    server_addr.sin_port = htons(PORT);  
  
    // create a stream socket   
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口  
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);  
    if (server_socket < 0)  
    {  
        printf("Create Socket Failed!\n");  
        exit(1);  
    }  
	printf("Create Socket Success!\n");  
  
	//注意：由TCP套接字状态TIME_WAIT引起在结束本次会话后close立刻开启下次会话会Bind失败。
	//该状态在套接字关闭后约保留 2 到 4 分钟。在 TIME_WAIT 状态退出之后，套接字被删除，该地址才能被重新绑定而不出问题。
	//因此下面两句话的加入可以解决这个问题
	int on = 1;
	setsockopt( server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

    // 把socket和socket地址结构绑定   
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))  
    {  
        printf("Binded success!\n");  
        exit(1);  
    }  
	printf("Create Socket Success!\n");  
	
    // server_socket用于监听   
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))  
    {  
        printf("Server Listen Failed!\n");  
        exit(1);  
    }  
	printf("server listen...\n");  
	
	// 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept  
	// 接受此请求，同时将client端的地址和端口等信息写入client_addr中  
	struct sockaddr_in client_addr;  
	socklen_t          length = sizeof(client_addr);  

	// 接受一个从client端到达server端的连接请求,将客户端的信息保存在client_addr中  
	// 如果没有连接请求，则一直等待直到有连接请求为止，这是accept函数的特性，可以  
	// 用select()来实现超时检测   
	// accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信  
	// 这里的new_server_socket代表了这个通信通道  
	int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);  
	if (new_server_socket < 0)  
	{  
		printf("Server Accept Failed!\n");  
		exit(1);  
	}  
	printf("listen连接客户端成功,new_server_socket = %d\n",new_server_socket);  
	printf("客户端ip =  %s\n",inet_ntoa(client_addr.sin_addr));
	printf("客户端端口号port = %d\n",ntohs(client_addr.sin_port));
		
    // 服务器端一直运行用以持续为客户端提供服务   
	char buffer[BUFFER_SIZE];  
    while(1)  
    {  
        bzero(buffer, sizeof(buffer));  
		printf("等待客户端发送过来的文件名：\n");
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);  
		if(length > 0)
		{
			printf("接收client消息成功:'%s'，共%d个字节的数据\n", buffer, length);
		}
        else
        {  
            printf("客户端断开了连接，退出！！！\n");  
            break;  
        }  
        char file_name[FILE_NAME_MAX_SIZE + 1];  
        bzero(file_name, sizeof(file_name));  
        strncpy(file_name, buffer,strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));  
  
        fp = fopen(file_name, "r");  
        if (fp == NULL)  
        {  
            printf("File:\t%s Not Found!\n", file_name);  
        }  
        else  
        {  
			printf("File:\t%s open success!\n", file_name);  
            bzero(buffer, BUFFER_SIZE);  
            int file_block_length = 0;  
			//循环将文件file_name(fp)中的内容读取到buffer中
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)  
            {  
                printf("读取到的文件长度file_block_length = %d\n", file_block_length);  
  
                // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端  
                if (send(new_server_socket, buffer, file_block_length, 0) < 0)  
                {  
                    printf("Send File:\t%s Failed!\n", file_name);  
                    break;  
                }  
				//清空buffer缓存区
                bzero(buffer, sizeof(buffer));  
            }  
            fclose(fp);  			//关闭文件描述符fp
            printf("File:\t%s Transfer Finished!\n", file_name);  
        }  
    }  
	close(new_server_socket);  		//关闭accept文件描述符
    close(server_socket);  			//关闭socket文件描述符
    return 0;  
}  