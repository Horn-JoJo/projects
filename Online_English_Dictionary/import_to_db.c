/*===============================================================
*   Copyright (C) 2021 All rights reserved.
*   
*   文件名称：import_to_db.c
*   创 建 者：liangjiawen
*   创建日期：2021年04月18日
*   描    述：
*
*   更新日志：
*
================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sqlite3.h>
#include <time.h>

int main(){
	time_t start,end;
	sqlite3 *db = NULL;

	if(0 != sqlite3_open("server.db", &db)){
		fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	printf("打开数据库......\n");
	printf("正在将单词表导入数据库......\n");
	start = time(NULL);
	char *errmsg = NULL;
	
	FILE *fp = fopen("dict.txt", "r");
	if(NULL == fp){
		perror("fopen");
		return -1;
	}
	
	char buf[1024];
	
	char *p = NULL;
	int i;
	char sql[1024];
	while(1){
		bzero(buf, 1024);
		if(NULL == fgets(buf, sizeof(buf), fp)){
			printf("read done!\n");
			end = time(NULL);
			printf("用时%.1fs.\n", difftime(end, start));
			break;
		}

		for(p = buf; *p != '\0'; p++)
			if(*p == ' ' && *(p + 1) != ' '){
				p++;
				break;
			}
		for(i = 0; buf[i] != '\0'; i++){
			if(buf[i] == ' '){
				buf[i] = '\0';
				break;
			}
		}
		bzero(sql, 1074);
		sprintf(sql, "insert into dictionary values('%s', '%s')", buf, p);
		if(0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)){
			fprintf(stderr, "sqlite3_exec:%s\n", errmsg);
			goto err;
		}

	}
	fclose(fp);
err:
	sqlite3_close(db);
	return 0;
}
