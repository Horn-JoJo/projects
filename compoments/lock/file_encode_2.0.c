#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<signal.h>

#define BUFSIZE 256
#define err_log(log)\
	do{\
		perror(log);\
		return -1;\
	}while(0);

char buf[BUFSIZE];
unsigned char key;
size_t sizes = 0;

static void encode(char buf[], int len, unsigned char key)
{
	for (int i = 0; i < len; i += 32) 
		if (((sizes + i) / 16) == 0) buf[i] ^= key;
	sizes += len;
}

void hander(int sig)
{
	printf("can not operator because encode/decode cant not init!!!!\n");
}

int main(int argc, char *argv[])
{
	signal(SIGINT, hander);
#if 1
	if (2 > argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME FILENAME\n");
		return -1;
	}
	int len;

	//读写打开源文件
	char *srcfile = argv[1];
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile");

	//再读写打开用来存储加密或者解密数据文件
	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again");

#endif
	printf("Please input a password(len<=10):");
	char psw[50] = {0}; 
	//scanf("%c", &key);
	scanf("%s", psw);
	//printf("psw = %s, len = %d\n", psw, strlen(psw));
	char *p = psw;

#if 1
	time_t t1 = time(NULL);
	while (*p != 0)
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), BUFSIZE, fp))
			{
				//读入len个字节数据并存到buf中，进行一个加密操作后，然后在写到新文件中
				encode(buf, len, *p);
				fwrite(buf, sizeof(char), len, newfp);
			}
			else break;
		}
	    fseek(newfp, 0, SEEK_SET);
	    fseek(fp, 0, SEEK_SET);
		sizes = 0;
		len = 0;
		memset(buf, 0, sizeof(buf));
		p++;
	}

	fclose(fp);
	fflush(newfp);
	fclose(newfp);
	time_t t2 = time(NULL);
	printf("用时%ld秒.\n", t2 - t1);
#endif
	return 0;
}
