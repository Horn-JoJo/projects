#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


typedef struct sockaddr SA;
#define err_log(log)\
	do{\
		perror(log);\
		exit(-1);\
	}while(0)

int main(int argc, const char *argv[])
{
	if (3 > argc)
	{
		fprintf(stderr, "FORMAT: ./APPNAME IP PORT\n");
		return -1;
	}
	socklen_t addrlen;
	struct sockaddr_in cliaddr;
	addrlen = sizeof(cliaddr);

	char buf[512];
	//填充网络地址
	struct sockaddr_in seraddr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = inet_addr(argv[1])
	};

	//创建数据报套接口
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	//将套接口和地址进行绑定
	if (0 > bind(sockfd, (SA *)&seraddr, sizeof(seraddr))) err_log("bind failed");

	while (1)
	{
		bzero(buf, sizeof(buf));
		if (0 > recvfrom(sockfd, buf, sizeof(buf), 0, (SA *)&cliaddr, &addrlen)) err_log("recvfrom failed");
		puts(buf);
		bzero(buf, sizeof(buf));

		snprintf(buf, sizeof(buf), "server:");
		int len = strlen(buf);
		gets(buf + len);
		if (0 > sendto(sockfd, buf, sizeof(buf), 0, (SA *)&cliaddr, addrlen)) err_log("sendto failed");
	}
	close(sockfd);
	return 0;
}
