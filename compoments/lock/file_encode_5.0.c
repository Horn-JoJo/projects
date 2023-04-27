#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<sqlite3.h>

#define BUFSIZE 256
#define err_log(log)\
	do{\
		perror(log);\
		return -1;\
	}while(0);

char buf[BUFSIZE];
size_t sizes = 0;

static void encode(char buf[], int len, unsigned char key)
{
	for (int i = 0; i < len; i += 32) 
		if (((sizes + i) / key) == 0) buf[i] ^= key;
	sizes += len;
}

void reverse(char *str) {
	int n = strlen(str); 
	
	for (int i = 0, j = n - 1; i < j; i++, j--) {
		int t = str[i];
		str[i] = str[j];
		str[j] = t; 
	}
}

int deal(char *srcfile, char *psw) 
{
	int len;
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile");

	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again");
	char *p = psw;
	
	time_t t1 = time(NULL);
	for (int i = 0; i < strlen(psw); i++)
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), BUFSIZE, fp))
			{
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

	return 0;
}

int main(int argc, char *argv[])
{
	if (3 !=  argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME FILENAME OPT\n");
		return -1;
	}

	sqlite3 *db = NULL;
	if (0 > sqlite3_open("./.lock.db", &db)) 
	{
		fprintf(stderr, "sqlite3_open database fail: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	// printf("sqlite3_open database sussessfully.\n");

	char *errmsg = NULL;
	char **resultp = NULL;
	int nrow;

	char sql[1024] = {0};
	snprintf(sql, 1024, "select psw, status from lock_msg where name = '%s'", argv[1]);
	if (0 != sqlite3_get_table(db, sql, &resultp, &nrow, NULL, &errmsg))
		fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);	

	if (1 == nrow) 
	{
		char *password = resultp[2];
		char *lock_status = resultp[3];

		if (strcmp(lock_status, "true") == 0 && '1' == argv[2][0]) 
		{
			printf("操作的文件已经加密！！！\n");
			return 0;
		}

		if (strcmp(lock_status, "false") == 0 && '2' == argv[2][0]) 
		{
			printf("操作的文件已经解密！！！\n");
			return 0;
		}

		if (strcmp(lock_status, "true") == 0 && '2' == argv[2][0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			
			if (strcmp(psw, password) != 0) {
				printf("请输入正确的密码！！！\n");
				return -1;
			}
			
			reverse(psw);
			if (0 == deal(argv[1], psw)) {
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "update lock_msg set status = 'false' where name = '%s'", argv[1]);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					return -1;
				}
				printf("解密成功！！\n");
			}
			else {
				printf("deal 函数出错！！！\n");
				return -1;
			}
	
			return 0;
		}

		if (strcmp(lock_status, "false") == 0 && '1' == argv[2][0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);

			if (0 == deal(argv[1], psw)) {
				//更新解密密码和解密状态
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "update lock_msg set psw = '%s', status = 'true' where name = '%s'", psw, argv[1]);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					return -1;
				}
				printf("加密成功！！\n");
			}
			else {
				printf("deal 函数出错！！！\n");
				return -1;
			}
			return 0;
		}
	}
	else if (0 == nrow)
	{
		if ('1' == argv[2][0])
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
		
			if (0 == deal(argv[1], psw)) {
				//更新解密密码和解密状态
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "insert into lock_msg values('%s', '%s' , 'true')", argv[1], psw);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					return -1;
				}
				printf("加密成功！！\n");
			}
			else {
				printf("deal 函数出错！！！\n");
				return -1;
			}
		}
		else if ('2' == argv[2][0])
		{
			memset(sql, 0, sizeof(sql));
			snprintf(sql, 1024, "insert into lock_msg values('%s', NULL , 'false')", argv[1]);
			if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				fprintf(stderr, "sqlite3_exec: %s\n", errmsg);
		}
	}

	sqlite3_close(db);
	return 0;
}