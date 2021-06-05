# file_socket 
基于socket实现文件传输

# 一.设计思路
&#8195;&#8195;基于socket实现server到client的文件传输，测试流程为client向server发起文件下载请求，同时将文件名发送给server；
server收到client的请求之后根据文件名到本地目录中查找相应文件，若是找到匹配的文件则使用fread将文件中的内容读出到buffer缓存中，使用scoket send函数发送给client，client收到数据之后将buffer中的内容使用fwrite函数写到文件中。整个过程使用到的基本操作就是：socket网络传输+文件操作。
# 二.编译及测试
编译：  
```
gcc send_file_server.c -o server    
gcc send_file_client.c -o client    
```
测试：   
&#8195;&#8195;1.在sever机器上与server同路径下创建test.txt文件并随意写入内容；    
&#8195;&#8195;2.服务端启动server程序如下：./server   
&#8195;&#8195;3.客户端启动client程序如下：./client   
&#8195;&#8195;4.client输入需要下载的文件名，如：test.txt   
