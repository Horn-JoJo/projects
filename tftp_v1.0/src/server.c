#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#define PATH "../res/"
#define MAX 1024
typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;
SAI seraddr, cliaddr;
socklen_t addrlen = sizeof(cliaddr);

char buf[MAX];
int confd;

void list_files()
{
	DIR *dirp = opendir(PATH);
	if (NULL == dirp)
	{
		perror("opendir");
		return;
	}

	memset(buf, 0, sizeof(buf));
	struct dirent *pcontex = NULL;
	while (1)
	{
		pcontex = readdir(dirp);
		if (NULL == pcontex) break;

		if (strncmp(pcontex->d_name, ".", 1) == 0 || strncmp(pcontex->d_name, "..", 2) == 0) continue;
		strcat(buf, pcontex->d_name);
		buf[strlen(buf)] = ' ';
	}
	send(confd, buf, strlen(buf), 0);
}

void get_file(char filename[])
{
	//printf("%s:filename : %s\n", __func__, filename);
	char path[128] = PATH;
	strcat(path, filename);
	FILE *fp = fopen(path, "r");
	if (NULL == fp)
	{
		perror("fopen");
		return;
	}

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int size = fread(buf, sizeof(char), MAX, fp);
		if (0 == size)
		{
			fclose(fp);
			break;
		}
		send(confd, buf, size, 0);
	}
}

void put_file(char order[], int len)
{
	//bug :当客户端输入错误的文件名，服务器也创建相应的空文件！！！
	//解决办法：调整client发送顺序
	char filename[64] = {0};	
	char path[128] = PATH;
	int i;
	for (i = 0; i < len; i++)
	{
		if (' ' == order[i]) break;
		filename[i] = order[i];
	}
	
	strcat(path, filename);
	FILE *fp = fopen(path, "w+");
	if (NULL == fp)
	{
		perror("fopen");
		return;
	}

	char *p = order + i + 1;
	fwrite(p, sizeof(char), len - i - 1, fp);

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int size =recv(confd, buf, MAX, 0);
		if (0 == size) break;
		fwrite(buf, sizeof(char), size, fp);
	}
	fclose(fp);
}

int main()
{

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > listenfd)
	{
		perror("socket");
		return -1;
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(9999);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 > bind(listenfd, (SA *)&seraddr, sizeof(seraddr)))
	{
		perror("bind");
		close(listenfd);
		return -1;
	}

	if (0 > listen(listenfd, 15))
	{
		perror("listen");
		close(listenfd);
		return -1;
	}

	char order[128];

	while (1)
	{
		confd = accept(listenfd, (SA *)&cliaddr, &addrlen);
		if (0 > confd)
		{
			perror("accept");
			close(listenfd);
			return -1;
		}

		memset(order, 0, sizeof(order));

		int size = recv(confd, order, sizeof(order), 0);
		if (0 == size) 
		{
			printf("peer exit.\n");
			close(confd);
			continue;
		}

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
	}
	close(listenfd);

	return 0;
}
