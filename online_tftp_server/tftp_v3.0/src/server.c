#include "../include/server.h"

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
