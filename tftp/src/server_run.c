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
		//考虑到客户端的数据会把命令和内容会依次性传送过来！！！
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

	/*使用poll监听可能发生的多个文件描述符事件并进行相应的处理*/
	//监听键盘
	int num = 0;
	//添加键盘文件描述符
	fds[num].fd = 0;
	fds[num++].events = POLLIN;//请求事件为POLLIN事件
	//添加监听文件描述符
	fds[num].fd = listenfd;
	fds[num++].events = POLLIN;
	/*=====================================*/
	// char order[128];
	addrlen = sizeof(cliaddr);

	//使用poll来
	while (1)
	{
		int ret = poll(fds, num, 10000);//没有事件产生就立即返回
		if (0 > ret)//函数调用错误 
		{
			perror("poll");
			break;
		}
		else if (0 == ret)//超时
		{
			printf("timeout...\n");
			continue;
		}
		else //有事件产生，则遍历是哪个文件描述符产生了事件
		{
			for (int i = 0; i < num; i++) 
			{
				if (fds[i].revents & POLLIN) 
				{
					if (0 == fds[i].fd) 
					{
						fgets(buf, 1024, stdin);//有换行
						//判断字符串是什么指令
						//然后执行什么样的操作
						fputs(buf, stdout);//这里先不进行扩展！！！
						memset(buf, 0, sizeof(buf));
					}
					else if (listenfd == fds[i].fd)
					{
						confd = accept(listenfd, (SA *)&cliaddr, &addrlen);
						printf("client[%s] connect successfully\n", inet_ntoa(cliaddr.sin_addr));

						if (0 > confd)
						{
							perror("accept");
							// close(listenfd);
							// return -1;
							continue;
						}
						/*===使用线程来处理客户端的请求，而主线程只是需要接收连接即可===*/	
						/*===这里来一个创建一个需要花时间，为了节省时间，可以改进为使用线程池，创建多个线程
						 * 开始都处于休眠状态，然后如果出现了需要的话，就进行唤醒来回调工作线程*/
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
				}
			}
		}
	}
	close(listenfd);
	return 0;
}
