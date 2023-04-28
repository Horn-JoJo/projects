#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;

#define MAX 1024
char buf[MAX];

#define err_deal(log, ERR)\
	do{\
		perror(log);\
		goto ERR;\
	}while(0)

void handler(int sig)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main()
{

	signal(SIGCHLD, handler);
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > listenfd) err_deal("socket", ERR);

	SAI srvaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(9999),
		.sin_addr.s_addr = inet_addr("192.168.0.104")
	};

	SAI cliaddr;
	socklen_t addrlen = sizeof(cliaddr);

	if (0 > bind(listenfd, (SA *)&srvaddr, sizeof(srvaddr))) err_deal("bind", ERR1);
	if (0 > listen(listenfd, 15)) err_deal("listen", ERR1);

	while (1)
	{
		int connfd = accept(listenfd, (SA *)&cliaddr, &addrlen);
		if (0 > connfd)
		{
			perror("accept");
			continue;
		}
		if (0 == fork())
		{
			//关闭子进程的监听套接字
			close(listenfd);

			while (1)
			{
				memset(buf, 0, sizeof(buf));
				int size = recv(connfd, buf, MAX, 0);
				if (0 > size)
				{
					perror("recv");
					break;
				}
				else if (0 == size)
				{
					printf("peer exit...\n");
					break;
				}
				send(connfd, buf, size, 0);			
				printf("recv: %s\n", buf);
			}
			close(connfd);
			memset(buf, 0, sizeof(buf));
			exit(0);
		}
		close(connfd);
	}
	close(listenfd);

	return 0;
ERR1:
	close(listenfd);
ERR:
	return -1;
}
