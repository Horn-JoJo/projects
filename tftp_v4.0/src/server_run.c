#include "../include/server.h"

void * worker(void * arg) 
{
	int confd = (int)arg;
	char order[128] = {0};
	// memset(order, 0, sizeof(order));
	//接收命令
	int size = recv(confd, order, sizeof(order), 0);
	if (0 == size) 
	{
		printf("peer exit.\n");
		close(confd);
		pthread_exit((void *)0);	
	}
	//判断命令是啥：有get put list
	if (strncmp(order, "get ", 4) == 0)
	{
		get_file(order + 4);
	}
	else if (strncmp(order, "put ", 4) == 0)
	{
		put_file(order + 4, size - 4);
	}
	else
	{
		list_files();
	}
	close(confd);
	pthread_exit((void *)0);
}

int main(int argc, const char * argv[])
{
	//1.创建socket 
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > listenfd)
	{
		perror("socket");
		return -1;
	}
	
	if (2 != argc) {
		fprintf(stderr, "FORMAT: ./APP IP\n");
		return -1;
	}
	//2.填充网络地址结构体
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(9999);
	seraddr.sin_addr.s_addr = inet_addr(argv[1]);

	//3.将socket与网络地址结构体进行绑定
	if (0 > bind(listenfd, (SA *)&seraddr, sizeof(seraddr)))
	{
		perror("bind");
		close(listenfd);
		return -1;
	}
	
	//4.设置监听
	if (0 > listen(listenfd, 15))
	{
		perror("listen");
		close(listenfd);
		return -1;
	}

	// char order[128];
	addrlen = sizeof(cliaddr);

	//多线程并发服务器模型
	while (1)
	{
		confd = accept(listenfd, (SA *)&cliaddr, &addrlen);
		if (0 > confd)
		{
			perror("accept");
			// close(listenfd);
			// return -1;
			continue;
		}
		/*===使用线程来处理客户端的请求，而主线程只是需要接收连接即可===*/	
		pthread_t tid;
		int ret = pthread_create(&tid, NULL, worker, (void *)confd);
		if (0 > ret) 
		{
			printf("pthread_create failed.\n");
			break;
		}
		pthread_detach(tid);
		/*==========================================================*/	
	}
	close(listenfd);

	return 0;
}
