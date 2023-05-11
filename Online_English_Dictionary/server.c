/*===============================================================
 *   Copyright (C) 2021 All rights reserved.
 *   
 *   文件名称：server.c
 *   创 建 者：liangjiawen
 *   创建日期：2021年05月11日
 *   描    述：
 *
 *   更新日志：
 *
 ================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;

void handler(int sig){
	while(waitpid(-1, NULL, WNOHANG) > 0); 
}


int main(int argc, char * argv[]){

	//处理子进程退出而产生僵尸
	signal(SIGCHLD, handler);

	//检验命令行参数
	if(3 > argc){
		fprintf(stderr, "SRV:%s, IP AND PORT!", argv[0]);
		return -1;
	}

	//创建套接字
	int srvfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > srvfd){
		perror("socket");
		return -1;
	}

	//定义IPV4地址结构并填充
	SAI srvaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = inet_addr(argv[1])
	};

	//允许重用地址和端口
	int on = 1;
	if(0 > setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
		perror("setsockopt");
		goto ERR;
	}

	//将套接字与该IP和端口绑定
	if(0 > bind(srvfd, (SA *)&srvaddr, sizeof(srvaddr))){
		perror("bind");
		goto ERR;
	}

	//设置为监听套接字
	if(0 > listen(srvfd, 20)){
		perror("listen");
		goto ERR;
	}

	SAI cliaddr;
	int addrlen = sizeof(cliaddr);
	char buf[1024];
	int ret;
	int clifd;
	while(1){
		//三次握手建立连接
		clifd = accept(srvfd, (SA *)&cliaddr, &addrlen);
		if(0 > clifd){
			perror("accept");
			continue;
		}else{
			printf("client : %s alreadly connect!\n", inet_ntoa(cliaddr.sin_addr));
		}

		//复制子进程
		if(0 == fork()) {
			close(srvfd);//子进程中关闭监听套接字，处理通信套接字的事务
			bzero(buf, sizeof(buf));
			ret = recv(clifd, buf, sizeof(buf), 0);
			puts(buf);
			if(0 > ret){
				perror("recv");
				// goto ERR1;
				close(clifd);
				exit(0);
			}

			if(0 == ret){
				printf("client: %s close...\n", inet_ntoa(cliaddr.sin_addr));
				// goto ERR1;
				close(clifd);
				exit(0);
			}

			//解析发送来的数据
			//LOG-ID:2021030114-PSW:12345678
			//登陆：id + psw 查找库后返回 查找结果 发送给客户端, 姓名呢？

			// ======================通信套接字处理逻辑==========================
			//1.接收客户端过来的ID和密码，进行用户验证
			if(strncmp(buf, "LOG", 3) == 0) {
				//解析id
				char id[12] = {0};
				char *p  = buf + 7;
				int i = 1;
				while(*p){
					if(*p == '-')
						break;
					i++;
					p++;
				}
				strncpy(id, buf + 7, i - 1);//跳过"LOG-ID:"

				//解析psw
				char psw[10] = {0};
				strcpy(psw, buf + 7 + (i - 1) + 5);

				//在数据库中查找对应的ID以及对应的密码，如果存在的话填充登录时间
				char logtime[50];
				
				//打开数据库
				sqlite3 *db = NULL;
				if(0 != sqlite3_open("server.db", &db) ){
					fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
					return -1;
				}

				char sql[1024] = {0};
				sprintf(sql, "select * from USER where id = '%s' and psw = '%s'", id, psw);

				//查找数据库返回查找信息
				char *errmsg = NULL;
				char **resultp = NULL;//指针数组首地址
				int nrow, ncolumn;

				if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
					fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
					sqlite3_close(db);
					return -1;
				}

				//登陆成功:发送信息到客户端表示登陆成功
				//添加 登陆ip + id + 登陆时间到log表中
				if(1 == nrow) { 
					bzero(buf, sizeof(buf));
					sprintf(buf, "SUCCESS");
					if(0 > send(clifd, buf, sizeof(sql), 0)){
						perror("send Login");
						// goto ERR1;
						close(clifd);
						exit(0);
					}
					//char logtime[200];
					time_t timep;
					struct tm *p;
					time(&timep);
					p = localtime(&timep);
					sprintf(logtime, "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
					// char sql2[1024] = {0};
					bzero(sql, sizeof(sql));
					sprintf(sql, "insert into log values('%s', '%s', '%s', 'NULL')", inet_ntoa(cliaddr.sin_addr), id, logtime);
					if(0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)){
						fprintf(stderr, "sqlite3_exec: %s\n", errmsg);
						sqlite3_close(db);
						return -1;
					} else {
						printf("记录成功！！！\n");
					}

					//客户端进入登陆成功后操作菜单，服务器接收search history命令
					//循环接收客户端发来的命令
					while(1){
						bzero(buf, sizeof(buf));
						ret = recv(clifd, buf, sizeof(buf), 0);
						if(0 > ret){
							perror("recv clifd order err ");
							// goto ERR1;
							close(clifd);
							exit(0);
						}
						if(0 == ret){
							printf("login_client log out\n\n");
							return 0;
						}
						//如果是查找单词：这里需要重新记录最近的查找时间！！！
						if(strncmp(buf, "search: ", 8) == 0){
							char word[20] = {0};
							strcpy(word, buf + 8);	
							bzero(sql, sizeof(sql));

							sprintf(sql, "select * from dictionary where word = '%s'", word);
							if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
								fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
								sqlite3_close(db);
								return -1; 
							}


							//成功会找到就发送到客户端查找的结果 并记录查询的单词到log表中
							if(1 == nrow){
								bzero(buf, sizeof(buf));
								sprintf(buf, "%s\n", resultp[1 * ncolumn + 1]);

								if(0 > send(clifd, buf, sizeof(buf), 0)){
									perror("send result to client err");
									goto ERR1;
								}

								bzero(sql, sizeof(sql));
								// 查找 当前ip id + logtime 的history
								sprintf(sql, "select *  from log where ip = '%s' and id = '%s' and logtime = '%s'", inet_ntoa(cliaddr.sin_addr), id, logtime);

								if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
									fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
									sqlite3_close(db);
									return -1; 
								}

								//这里存在一个问题：当查询的新的单词时，只会将最近查找的单词追加到末尾，而最近的访问时间没有更新
								//查找到history 追加新单词到history中（如果==NULL 直接覆盖，否则连接）
								if(1 == nrow){
									char his[512] = {0};
									strcpy(his, resultp[ncolumn + 3]);//第二行的第四列的值
									if(strncmp(resultp[1 * ncolumn + 3], "NULL", 4) == 0){
										//printf("word :%s\n", word);
										strcpy(his, word);
										printf("searched word: %s\n", his);
									}else{
										strcpy(his, resultp[1 * ncolumn + 3]);
										his[strlen(his)] = ';';
										strcat(his + strlen(his), word);
										printf("searched word: %s\n", his);
										//printf("his: %s\n", his);
									}
									bzero(sql, sizeof(sql));
									sprintf(sql, "update log set history = '%s'where ip = '%s' and id = '%s' and logtime = '%s' ", his, inet_ntoa(cliaddr.sin_addr), id, logtime);
									if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
										fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
										sqlite3_close(db);
										return -1;
									}

								} else {
									printf("select current hissory err!\n");
								}



							}else{//否则发送没有找到该单词
								bzero(sql, sizeof(sql));
								sprintf(sql, "not find this word!(0000 for exit search)\n\n");
								if(0 > send(clifd, sql, sizeof(sql), 0)){
									perror("send result to client err");
									goto ERR1;
								}
							}


						//登陆成功的状态下收到指令history查看历史记录
						}else if(strncmp(buf, "history", 7) == 0){
							bzero(sql, sizeof(sql));
							sprintf(sql, "select * from log where id = '%s'", id);
							if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
								fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
								sqlite3_close(db);
								return -1; 
							}

							if(0 != nrow){
								int j;
								int len = 0;
								//int count = 0
								bzero(buf, sizeof(buf));
								for(i =  0; i < nrow + 1; i++){//nrow:查找到的记录条数
									for(j = 0; j < ncolumn; j++){
										//printf("%-20s", resultp[i * ncolumn + j]);//resultp[count++] 1*ncolumn + 1
										if(j == ncolumn - 1)
											sprintf(buf + len, "%-20s\n", resultp[i * ncolumn + j]);
										else
											sprintf(buf + len, "%-20s", resultp[i * ncolumn + j]);
										len = strlen(buf);
									}
									//printf("\n");
								}
								if(0 > send(clifd, buf, sizeof(buf), 0)){
									perror("send result to client err");
									goto ERR1;
								}
							}else{
								bzero(sql, sizeof(sql));
								sprintf(sql, "not find hissory!(0000 for exit)\n\n");
								if(0 > send(clifd, sql, sizeof(sql), 0)){
									perror("send result to client err");
									goto ERR1;
								}
							}


						}else{
							printf("error!\n");
						}
					}
					//关闭数据库
					sqlite3_close(db);

				}else{//失败
					bzero(sql, sizeof(sql));
					sprintf(sql, "ERR");
					if(0 > send(clifd, sql, sizeof(sql), 0)){
						perror("send Login");
						goto ERR1;
					}
					printf("Login connect exit.\n\n");
					close(clifd);
					exit(0);
				}
				
			}else if (strncmp(buf, "REG", 3) == 0){//注册
				//REG-NAME:liangjiawen-SEX:m-ID:2021030114-PSW:12345678
				//解析数据
				char name[17] = {0};
				char *p = buf + 9;
				int i = 0;
				while(*p){
					if(*p == '-')
						break;
					i++;
					p++;
				}
				*p = '0';
				strncpy(name, buf + 9, i);


				char sex[7] = {0};
				p = p + 5;
				sex[0] = *p;

				char id[12] = {0};
				p += 5;
				i = 1;
				while(*(p + i)){
					if(*(p + i) == '-')	
						break;
					i++;
				}
				strncpy(id, p, i);

				char pwd[10] = {0};
				p += (i + 5);
				strcpy(pwd, p);


				//打开数据库

				sqlite3 *db = NULL;
				if(0 != sqlite3_open("server.db", &db) ){
					fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
					return -1;
				}
				//查找是否已经ID一样的的数据，有就发送已经存在该ID，否则插入数据，插入成功发送成功信息
				char *errmsg = NULL;
				char **resultp = NULL;//指针数组首地址
				int nrow, ncolumn;

				char sql[256] = {0};
				char buf[1024] = {0};

				//先查找验证
				sprintf(sql, "select * from USER where id = '%s'", id);


				if(0 != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)){
					fprintf(stderr, "sqlite3_get_table: %s\n", errmsg);
					sqlite3_close(db);
					return -1; 
				}

				if(1 == nrow){
					bzero(buf, sizeof(buf));
					sprintf(buf, "ID alreadly exists! regist fail!");
					if(0 > send(clifd, buf, sizeof(buf), 0)){
						perror("send");
						goto ERR1;
					}
				}else{
					bzero(sql, sizeof(sql));
					sprintf(sql, "insert into USER values('%s', '%s', '%s', '%s')", name, sex, id, pwd);
					if(0 != sqlite3_exec(db, sql, NULL, NULL, &errmsg)){
						fprintf(stderr, "sqlite3_exec: %s\n", errmsg);
						sqlite3_close(db);
						exit(0);
					}

					bzero(buf, sizeof(buf));
					sprintf(buf, "Register success!");
					if(0 > send(clifd, buf, sizeof(buf), 0)){
						perror("send");
						goto ERR1;
					}

				}
				//在根据情况

				bzero(buf, sizeof(buf));
				if(0 == recv(clifd, buf, sizeof(buf), 0)){
					printf("regist connect exit.\n\n");
					goto ERR1;

				}

				//关闭数据库
				sqlite3_close(db);
			}

ERR1:
			{
				close(clifd);
				exit(0);
			}

		}
		close(clifd);//父进程中关闭通信套接字，只进行建立连接的过程
	}
	close(srvfd);
	return 0;
ERR: 
	close(srvfd);
	return -1;
}
