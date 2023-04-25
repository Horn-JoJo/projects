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
		if (((sizes + i) / key) == 0) buf[i] ^= key;
	sizes += len;
}

void hander(int sig)
{
	printf("can not operator because encode/decode cant not init!!!!\n");
}

int main(int argc, char *argv[])
{
	signal(SIGINT, hander);
	if (2 > argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME FILENAME\n");
		return -1;
	}

	/*
	首先判断当前文件是否操作过(通过数据库来存储)
		需要存储 文件名  密码 加密状态(默认是false)
		
		打开数据库，查找有无待操作当前文件的状态记录
		1.如果有
	    	获取对应的加密状态。
		   1)如果当前指令是加密操作，并且已经是加密状态的话。则直接提醒用户该文件已经加密！！！
		   2)如果当前指令是解密操作，并且已经是解密状态的话。则直接提醒用户该文件是解密状态！！！
		   3)如果当前指令是加密操作，现在处于未加密状态， 则可以指定密码新的密码来进行加密!!! 
		     加密成功后更新操作密码和状态（未加密状态->加密状态）
		   4)如果当前指令时解密操作，现在处于加密状态，则必须验证密码的正确性
		     表中的存储的字符串密码是否是当前输入的字符串密码倒序过来的串！！！
			 如果是，则解密密码正确，然后更新状态到表中(加密状态->未加密状态)
		2.如果没有(第一次操作)
			在文件在数据库表中新增一条操作当前文件的一个状态记录
			如果是加密，则进行加密
		    如果是解密，提醒用户对应该文件尚且没有任何加密解密操作，然后添加记录(文件名  无密码 未加密状态)
	*/



	int len;
	//读写打开源文件
	char *srcfile = argv[1];
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile");


	//再读写打开用来存储处理后数据文件
	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again");

	printf("Please input a password(len<=50):");
	char psw[51] = {0}; 
	scanf("%s", psw);
	char *p = psw;
	
	time_t t1 = time(NULL);
	for (int i = 0; i < strlen(psw); i++) //挨个取出每个密码的字符，对数据进行加密
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), BUFSIZE, fp))
			{
				//读入len个字节数据并存到buf中，进行一个加密操作后，然后在写到新文件中
				encode(buf, len, (psw[i] + i) % 128);//字符加密一段数据
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
	

	/*加密成功/或者解密成功！！！
	如果


	*/
	fclose(fp);
	fflush(newfp);
	fclose(newfp);
	time_t t2 = time(NULL);
	printf("用时%ld秒.\n", t2 - t1);

	return 0;
}
