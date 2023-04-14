#include "lock.h"

/*char buf[BUFSIZE];//在net.h中已经共有了个buf的声明, 大小为MAX*/
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

void do_lock()
{
	char file[128] = {0};
	printf("Plesase a file path:");
	scanf("%s", file);

	int len;
	//读写打开源文件
	
	char *srcfile = file;
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile", RET);

	//再读写打开用来存储加密或者解密数据文件
	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again", RET);

	signal(SIGINT, hander);
	
	getchar();
	printf("Please input a password(len<=50):");
	char psw[MAX] = {0}; 
	scanf("%s", psw);
	getchar();//吸收垃圾字符
	char *p = psw;
	
	time_t t1 = time(NULL);
	for (int i = 0; i < strlen(psw); i++) 
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), MAX, fp))
			{
				//读入len个字节数据并存到buf中，进行一个加密操作后，然后在写到新文件中
				//encode(buf, len, (psw[i] + i) % 128);有问题！！！
				encode(buf, len, psw[i]);
				fwrite(buf, sizeof(char), len, newfp);
			}
			else break;
		}
	    fseek(newfp, 0, SEEK_SET);
	    fseek(fp, 0, SEEK_SET);
		sizes = 0;
		len = 0;
		memset(buf, 0, sizeof(buf));
	}

	fclose(fp);
	fflush(newfp);
	fclose(newfp);
	time_t t2 = time(NULL);
	printf("用时%ld秒.\n", t2 - t1);

RET:
	return;
}
