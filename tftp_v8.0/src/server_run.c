#include "../include/server.h"
#include "../include/thr.h"
#include "../include/thrpool.h"

//让其作为工作函数，而不是直接作为线程函数
//用已经初始化就创建好的线程来回调该函数(线程池)
//而不是每次连接一个创建一个线程，这样会浪费资源
//线程工作函数---接收来自客户端的连接，根据输入的指令进行不同逻辑
void * worker(void * arg) 
{
	thrpool_t *this = (thrpool_t *)arg;
	int confd = this->fd;
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
		list_files();//修改哈list的处理逻辑
	}
	close(confd);
	
	//让线程池的该线程失效，挂起。
	this->fd = INVALIDFD;
	thr_susend(&(this->thr));
	//pthread_exit((void *)0);
}

int server_init(char IP[]) 
{
	
	//1.创建socket 
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > listenfd) err_log("socket", ERR);

	//2.允许网络地址重用
	int on = 1;
	if (0 > setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) err_log("setsockopt", ERR1);

	//3.填充网络地址结构体
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(9999);
	seraddr.sin_addr.s_addr = inet_addr(IP);

	//4.将socket与网络地址结构体进行绑定
	if (0 > bind(listenfd, (SA *)&seraddr, sizeof(seraddr))) err_log("bind", ERR1);
	
	//5.设置监听
	if (0 > listen(listenfd, 15)) err_log("listen", ERR1);

	return listenfd;

ERR1:
	close(listenfd);
ERR:
	return -1;
}

int main(int argc, const char * argv[])
{
	if (2 != argc) {
		fprintf(stderr, "FORMAT: ./APP IP\n");
		return -1;
	}

	int listenfd = server_init(argv[1]);
	if (0 > listenfd) 
	{
		printf("server_init failed...\n");
		goto ERR;
	}

	/*===初始化线程池:开启15个线程，并且每个线程对象都是注册的worker工作函数，连接套接字处于无效状态，且线程都处于挂起状态===*/
	thrpool_init(worker);
	/*=========================================================*/

	/*使用poll监听可能发生的多个文件描述符事件并进行相应的处理: 轮询检测*/
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
//这里可以编译的指定这个宏 -DMYBUG
//这样就会来一个连接就创建一个线程来处理一次操作
#if 0
						/*===使用线程来处理客户端的请求，而主线程只是需要接收连接即可===*/	
						pthread_t tid;
						int ret = pthread_create(&tid, NULL, worker, (void *)confd);
						if (0 > ret) 
						{
							printf("pthread_create failed.\n");
							break;
						}
						pthread_detach(tid);
#else					
						/*==========================================================*/	
						/*===这里来一个创建一个需要花时间，为了节省时间，可以改进为使用线程池，创建多个线程
						 * 开始都处于休眠状态，然后如果出现了需要的话，就进行唤醒来回调工作线程*/
						//获取线程池中空闲的线程
						int id = get_thr_id();
						if (0 > id) {
							close(confd);
							printf("no more thread to be operatered...\n");
							break;
						}
						pool[id].fd = confd;
						thr_resume(&(pool[id].thr));
						/*==========================================================*/	
#endif
					}
				}
			}
		}
	}

	close(listenfd);
	return 0;
ERR1:
	close(listenfd);
ERR:
	return -1;
}
