#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;

#define MAX 1024
char sndbuf[MAX];
char rcvbuf[MAX];

#define err_deal_with(err_log, ORDER)\
	do{\
		perror(err_log);\
		goto ORDER;\
	}while(0)

int main()
{
	int connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > connfd) err_deal_with("socket", ERR);

	SAI srvaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(9999),
		.sin_addr.s_addr = inet_addr("192.168.0.104")
	};

	if (0 > connect(connfd, (SA *)&srvaddr, sizeof(srvaddr))) err_deal_with("connect", ERR1);

	while (1)
	{
		fgets(sndbuf, MAX, stdin);
		sndbuf[strlen(sndbuf) - 1] = 0;
		if (strncmp(sndbuf, "quit", 4) == 0) break;
		if (0 > send(connfd, sndbuf, strlen(sndbuf), 0))
		{
			perror("send");
			break;
		}
		recv(connfd, rcvbuf, sizeof(rcvbuf), 0);
		puts(rcvbuf);

		memset(sndbuf, 0, sizeof(sndbuf));
		memset(rcvbuf, 0, sizeof(rcvbuf));
	}

ERR1:
	close(connfd);
ERR:
	return 0;
}
