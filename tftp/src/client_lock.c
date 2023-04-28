#include "lock.h"

static void encode(char buf[], int len, unsigned char key)
{
	for (int i = 0; i < len; i += 32) 
		if (((sizes + i) / key) == 0) buf[i] ^= key;
	sizes += len;
}

static void reverse(char *str) {
	int n = strlen(str); 
	
	for (int i = 0, j = n - 1; i < j; i++, j--) {
		int t = str[i];
		str[i] = str[j];
		str[j] = t; 
	}
}

static int deal(char *srcfile, char *psw) 
{
	int len;
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile", ERR);

	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again", ERR);
	/*char *p = psw;*/
	
	time_t t1 = time(NULL);
	for (int i = 0; i < strlen(psw); i++)
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), MAX, fp))
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
ERR:
	return -1;
}

void do_lock()
{
	/*if (3 !=  argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME FILENAME OPT\n");
		return -1;
	}*/
	char filename[MAX] = {0};
	char op[2] = {0};
	printf("Please input a filename and one opeaation.\n");
	scanf("%s%s", filename, op);
	getchar();

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
	snprintf(sql, 1024, "select psw, status from lock_msg where name = '%s'", filename);
	if (0 != sqlite3_get_table(db, sql, &resultp, &nrow, NULL, &errmsg))
		fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);	

	if (1 == nrow) 
	{
		char *password = resultp[2];
		char *lock_status = resultp[3];

		if (strcmp(lock_status, "true") == 0 && '1' == op[0]) 
			print("操作的文件已经加密！！！\n", RET);

		if (strcmp(lock_status, "false") == 0 && '2' == op[0]) 
			print("操作的文件已经解密！！！\n", RET);

		if (strcmp(lock_status, "true") == 0 && '2' == op[0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			getchar();
			
			if (strcmp(psw, password) != 0) 
				print("请输入正确的密码！！！\n", RET);
			
			reverse(psw);
			if (0 == deal(filename, psw)) {
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "update lock_msg set status = 'false' where name = '%s'", filename);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					goto RET;
				}
				printf("解密成功！！\n");
			}
			else print("deal 函数出错！！！\n", RET);
		}

		else if (strcmp(lock_status, "false") == 0 && '1' == op[0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			getchar();

			if (0 == deal(filename, psw)) {
				//更新解密密码和解密状态
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "update lock_msg set psw = '%s', status = 'true' where name = '%s'", psw, filename);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					goto RET;
				}
				printf("加密成功！！\n");
			}
			else print("deal 函数出错！！！\n", RET);
		}
	}
	else if (0 == nrow)
	{
		if ('1' == op[0])
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			getchar();
		
			if (0 == deal(filename, psw)) {
				//更新解密密码和解密状态
				memset(sql, 0, sizeof(sql));
				snprintf(sql, 1024, "insert into lock_msg values('%s', '%s' , 'true')", filename, psw);
				if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				{
					fprintf(stderr, "sqlite3_exec-update: %s\n", errmsg);
					goto RET;
				}
				printf("加密成功！！\n");
			}
			else print("deal 函数出错！！！\n", RET);
		}
		else if ('2' == op[0])
		{
			memset(sql, 0, sizeof(sql));
			snprintf(sql, 1024, "insert into lock_msg values('%s', NULL , 'false')", filename);
			if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				fprintf(stderr, "sqlite3_exec: %s\n", errmsg);
		}
	}

RET:
	sqlite3_close(db);
	return;
}
