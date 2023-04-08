#include<stdio.h>

#define BUFSIZE 256
char buf[BUFSIZE];

int main(int argc, const char *argv[])
{
	if (2 > argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME SRCPATH NEWFILE\n");
		return -1;
	}
	int len, size;

	//只读打开源文件
	char *srcpath = argv[1];
	FILE *sfp = fopen(srcpath, "r");

	if (NULL == sfp)
	{
		perror("fopen srcpath");
		return -1;
	}

	
	//创建打开空文件
	char *newfile = argv[2];
	FILE *dfp = fopen(newfile, "a+");
	if (NULL == dfp)
	{
		perror("fopen newfile");
		return -1;
	}

	while (1)
	{
		if(len = fread(buf, sizeof(char), BUFSIZE, sfp))
		{
			fwrite(buf, sizeof(char), len, dfp);
		}
		else
		{
			printf("fwrite newfile done.\n");
			break;
		}	
	}

	fflush(dfp);
	fclose(sfp);
	fclose(dfp);
	return 0;
}
