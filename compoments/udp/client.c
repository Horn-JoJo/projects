#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct sockaddr SA;
#define err_log(log)\
	do{\
		perror(log);\
		exit(-1);\
	}while (0)

int main(int argc, const char *argv[])
{
	if (3 > argc)
	{
		fprintf(stderr, "FORMAT ./APP IP PORT.\n");
		return -1;
	}

	char buf[1024];
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in seraddr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = inet_addr(argv[1])
	};

	while (1)
	{
		bzero(buf, sizeof(buf));
		snprintf(buf, sizeof(buf), "client:");
		int len = strlen(buf);
		gets(buf + len);
		if (0 > sendto(sockfd, buf, sizeof(buf), 0, (SA *)&seraddr, sizeof(seraddr))) err_log("sendto failed");

		bzero(buf, sizeof(buf));
		if (0 > recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL)) err_log("recvfrom");
		puts(buf);
	}

	close(sockfd);
	return 0;
}
