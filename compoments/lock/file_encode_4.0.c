#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<signal.h>
#include<sqlite3.h>

#define BUFSIZE 256
#define err_log(log)\
	do{\
		perror(log);\
		return -1;\
	}while(0);

char buf[BUFSIZE];
// unsigned char key;
size_t sizes = 0;

static void encode(char buf[], int len, unsigned char key)
{
	for (int i = 0; i < len; i += 32) 
		if (((sizes + i) / key) == 0) buf[i] ^= key;
	sizes += len;
}

// void hander(int sig)
// {
// 	printf("can not operator because encode/decode cant not init!!!!\n");
// }
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
	//读写打开源文件
	// char *srcfile = argv[1];
	FILE *fp = fopen(srcfile, "r+");
	if (NULL == fp) err_log("fopen srcfile");


	//再读写打开用来存储处理后数据文件
	FILE *newfp = fopen(srcfile, "r+");
	if (NULL == newfp) err_log("fopen srcfile again");

	// printf("Please input a password(len<=50):");
	// char psw[51] = {0}; 
	// scanf("%s", psw);
	char *p = psw;
	
	time_t t1 = time(NULL);
	for (int i = 0; i < strlen(psw); i++) //挨个取出每个密码的字符，对数据进行加密
	{
		while (1)
		{
			if(len = fread(buf, sizeof(char), BUFSIZE, fp))
			{
				//读入len个字节数据并存到buf中，进行一个加密操作后，然后在写到新文件中
				// encode(buf, len, (psw[i] + i));//字符加密一段数据
				encode(buf, len, psw[i]);//字符加密一段数据
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
	// signal(SIGINT, hander);
	if (3 !=  argc)
	{
		fprintf(stderr, "INPUT FORMAT:APPNAME FILENAME OPT\n");
		return -1;
	}

	/*
	首先判断当前文件是否操作过(通过数据库来存储)
		需要存储 文件名  密码 加密状态(默认是false)
				 text    text bool	


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

	//首先打开数据库查找是否有它同一个文件名的条目
	//打开数据库

	// printf("Please input a password(len<=50):");
	// char psw[51] = {0}; 
	// scanf("%s", psw);

	sqlite3 *db = NULL;
	if (0 > sqlite3_open("./.lock.db", &db)) 
	{
		fprintf(stderr, "sqlite3_open database fail: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	
	printf("sqlite3_open database sussessfully.\n");

	//查找是否有操作文件的的条目
	char *errmsg = NULL;
	char **resultp = NULL;
	int nrow;
	// ncolumn;

	char sql[1024] = {0};
	snprintf(sql, 1024, "select psw, status from lock_msg where name = '%s'", argv[1]);
	if (0 != sqlite3_get_table(db, sql, &resultp, &nrow, NULL, &errmsg))
		fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);	

	if (1 == nrow) 
	{// 判断查找到，就提取出信息， nrow必然是1，因为主键是唯一的！！！
		// printf("%-15s", resultp[2]);//对应的密码，如果是没有密码，则返回的是(null)
		// printf("%-15s", resultp[3]);//对应的加密状态, 已经加密则返回的数true字符串
		// puts("");

		char *password = resultp[2];
		char *lock_status = resultp[3];

		// 3)如果当前指令是加密操作，现在处于未加密状态， 则可以指定密码新的密码来进行加密!!! 
		// 	加密成功后更新操作密码和状态（未加密状态->加密状态）
		// 4)如果当前指令时解密操作，现在处于加密状态，则必须验证密码的正确性
		// 	表中的存储的字符串密码是否是当前输入的字符串密码倒序过来的串！！！
		// 	如果是，则解密密码正确，然后更新状态到表中(加密状态->未加密状态)
		// printf("%-15s", password);
		// printf("%-15s", lock_status);

		if (strcmp(lock_status, "true") == 0 && '1' == argv[2][0]) 
		{
			printf("操作的文件已经加密！！！");
			return 0;
		}

		if (strcmp(lock_status, "false") == 0 && '2' == argv[2][0]) 
		{
			printf("操作的文件已经解密！！！");
			return 0;
		}

		//加密状态，想要解密
		if (strcmp(lock_status, "true") == 0 && '2' == argv[2][0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			
			//需要判断是否倒序的密码串是否跟存储的密码值是否相等
			if (strcmp(psw, password) != 0) {
				printf("请输入正确的密码！！！");
				return -1;
			}
			
			reverse(psw);
			//调用编码逻辑
			if (0 == deal(argv[1], psw)) {
				//更新解密密码和解密状态
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

		//解密状态 想要加密
		if (strcmp(lock_status, "false") == 0 && '1' == argv[2][0]) 
		{
			printf("Please input a password(len<=50):");
			char psw[51] = {0}; 
			scanf("%s", psw);
			//需要判断密码串存储的密码值是否相等
			
			//调用编码逻辑
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
		//没有查询到
		printf("not found\n");	

	    //需要添加条目
		if ('1' == argv[2][0]) //如果是加密操作
		{
			//只需要添加操作信息到表中，而不需要进行解密操作
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
		else if ('2' == argv[2][0]) //如果是解密操作
		{
			//只需要添加操作信息到表中，而不需要进行解密操作
			memset(sql, 0, sizeof(sql));
			snprintf(sql, 1024, "insert into lock_msg values('%s', NULL , 'false')", argv[1]);
			if (0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)) 
				fprintf(stderr, "sqlite3_exec: %s\n", errmsg);
		}
	}

	//关闭数据库
	sqlite3_close(db);
	return 0;
}