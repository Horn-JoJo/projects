#include "../include/client.h"

void do_get(char order[])
{
	//这里需要找空格后出现的第一个字符位置,
	//根据输入的逻辑, get指令后必然有一个空格
	char *file = order + 4;
	while (*file != 0) 
	{
		if (*file != ' ') break;
		file++;
	}

	if (*file == ' ') 
	{
		printf("Please input a file name!!!\n");
		goto ERR;
	}

	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd) 	err_log("do_get:socket", ERR);
	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr))) err_log("do_get:connect", ERR1);
	if (0 > send(confd, order, strlen(order), 0))err_log("do_get:send", ERR1);

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
			fp = fopen(file, "a+");
			if (NULL == fp)
			{
				perror("fopen");
				break;
			}
		}

		fwrite(buf, sizeof(char), size, fp);
		fflush(fp);
	}
ERR1:
	close(confd);
ERR:
	return;
}

void do_put(char order[])
{
	//这里需要找空格后出现的第一个字符位置,
	//根据输入的逻辑, put指令后必然有一个空格
	char *file = order + 4;
	while (*file != 0) 
	{
		if (*file != ' ') break;
		file++;
	}
	if (*file == ' ') 
	{
		printf("Please input a file name!!!\n");
		goto ERR;
	}

	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd) err_log("do_put:socket", ERR);
	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr))) err_log("do_put:connect", ERR1);
	
	FILE *fp = fopen(file, "r");
	//FILE *fp = fopen(order + 4, "r");
	if (NULL == fp)
	{
		printf("file not found.\n");
		goto ERR1;
	}
	
	strcpy(buf, order);
	buf[strlen(order)] = '#';//命令的结束标识
	send(confd, buf, strlen(buf), 0);

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
ERR1:
	close(confd);
ERR:
	return;
}

void do_list(char cmd[])
{
	memset(buf, 0, sizeof(buf));
	confd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > confd) err_log("do_list:socket", ERR);//直接return
	if (0 > connect(confd, (SA *)&seraddr, sizeof(seraddr))) err_log("do_list:connect", ERR1);

	send(confd, cmd, strlen(cmd), 0);

	int size = 0;
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		size = recv(confd, buf, sizeof(buf), 0);
		if (0 == size) break;
		puts(buf);
	}
ERR1:
	close(confd);
ERR:
	return;
}

void do_help()
{
	puts("usage:");
	puts("list         show the files lists of server path");
	puts("get file     download the file named file to client");
	puts("put file     upload the file named file to server");
	puts("clear        clear the system output");
	puts("en/decrypt   encrypt or decrypt a file");
	puts("help         show the commands that can be used");
	puts("exit         exit the client application");
}
