#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 128
#define MAX 1024
int confd = -1;
char buf[MAX];
typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;
SAI seraddr;

#define err_log(log)\
	do{\
		perror(log);\
		return;\
	}while(0)

void do_get(char order[])
{
	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd)
	{
		perror("do_get:socket");
		return;
	}
	
	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr)))
	{
		perror("do_get:connect");
		close(confd);
		return;
	}

	if (0 > send(confd, order, strlen(order), 0))
	{
		perror("do_get:send");
		close(confd);
		return;
	}

	int isExist = 0;
	FILE *fp = NULL;

	while (1)
	{
		int size = recv(confd, buf, sizeof(buf), 0);
		//这有两种情况：
		//1.没有找到文件直接对方关闭套接字
		//2.发送完成时对方关闭套接字
		if (0 == size)
		{
			if (isExist) 
			{
				printf("get successfully.\n");
				fclose(fp);
				break;
			}
			printf("file not found.\n");
			break;	
		}

		isExist = 1;
		if (NULL == fp)
		{
			fp = fopen(order + 4, "a+");
			if (NULL == fp)
			{
				perror("fopen");
				break;
			}
		}

		fwrite(buf, sizeof(char), size, fp);
		fflush(fp);
	}
	close(confd);
}

void do_put(char order[])
{
	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd) 
	{
		perror("do_put:socket");
		return;
	}

	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr)))
	{
		perror("do_put:connect");
		return;
	}
	
	strcpy(buf, order);
	buf[strlen(order)] = ' ';
	send(confd, buf, strlen(buf), 0);

	FILE *fp = fopen(order + 4, "r");
	if (NULL == fp)
	{
		perror("fopen");
		close(confd);
		return;
	}
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int size = fread(buf, sizeof(char), MAX, fp);
		if (0 == size)
		{
			printf("put successfully.\n");
			fclose(fp);
			break;
		}
		send(confd, buf, size, 0);
	}
	close(confd);
}

void do_list(char cmd[])
{
	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd) err_log("do_list:socket");
	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr))) err_log("do_list:connect");

	send(confd, cmd, strlen(cmd), 0);

	int size = 0;
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		size = recv(confd, buf, sizeof(buf), 0);
		if (0 == size) break;
		puts(buf);
	}
	close(confd);
}

void do_help()
{
	puts("usage:");
	puts("list         show the files lists of server path");
	puts("get file     download the file named file to client");
	puts("put file     upload the file named file to server");
	puts("clear        clear the system output");
	puts("help         show the commands that can be used");
	puts("exit         exit the client application");
}



int main(int argc, const char* argv[])
{
	if (3 > argc)
	{
		fprintf(stderr, "FORMAT: ./APP IP PORT");
		return -1;
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(atoi(argv[2]));
	seraddr.sin_addr.s_addr = inet_addr(argv[1]);

	char order[SIZE];

	while (1)
	{
		printf(">>tftp:");
		fgets(order, SIZE, stdin);
		order[strlen(order) - 1] = 0;

		if (strncmp(order, "get ", 4) == 0)
			do_get(order);
		else if (strncmp(order, "put ", 4) == 0)
			do_put(order);
		else if (strncmp(order, "list", 4) == 0)
			do_list(order);
		else if (strncmp(order, "help", 4) == 0)
			do_help();
		else if (strncmp(order, "exit", 4) == 0)
			exit(0);
		else if (strncmp(order, "clear", 5) == 0)
			system("clear");
		else printf("%s: command can not found.\n", order);
		memset(order, 0, sizeof(order));
	}

	return 0;
}
