/*===============================================================
 *   Copyright (C) 2021 All rights reserved.
 *   
 *   文件名称：client.c
 *   创 建 者：JOJO
 *   创建日期：2023年05月11日
 *   描    述：
 *
 *   更新日志：
 *
 ================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;

void clientMenu(){
	puts("==============================");
	puts("=--------Client Menu---------=");
	puts("=--1-----Login---------------=");
	puts("=--2-----Register------------=");
	puts("=--0-----Quit----------------=");
	puts("==============================");
}

void serverMenu(){

	puts("==============================");
	puts("=---------Server Menu--------=");
	puts("=--1------Search word--------=");
	puts("=--2------History------------=");
	puts("=--0------Quit---------------=");
	puts("==============================");
}

//连接服务器
int connect_server(int sockfd, SAI srvaddr){

	if(0 > connect(sockfd, (SA *)&srvaddr, sizeof(srvaddr))){
		perror("connect");
		return -1;
	}else{
		printf("connect to server!\n");
	}

	return 0;
}

int login(int sockfd){

	char buf[1024] = {0};
	char ID[12] = {0};//最后两位\n\0
	char PSW[10] = {0};

	printf("ID:");
	fgets(ID, sizeof(ID), stdin);
	ID[strlen(ID) - 1] = 0;
	printf("PSW:");
	fgets(PSW, sizeof(PSW), stdin);
	PSW[strlen(PSW) - 1] = 0; 

	sprintf(buf, "LOG-ID:%s-PSW:%s", ID, PSW);
	printf("ID = %s PSW = %s\n", ID, PSW);

	if(0 > send(sockfd, buf, sizeof(buf), 0)){
		perror("send logininfo to server err!");
		return -1;
	}

	int ret = recv(sockfd, buf, sizeof(buf), 0);
	if(0 > ret){
		perror("recv login status err!");
		return -1;
	}

	//printf("Login status: %s\n", buf);

	if(strncmp(buf, "SUCCESS", 7) != 0){
		printf("Login fail!\n");
		return -1;

	}
	printf("Login success!\n");
	return 0;
}

int Reg(int sockfd){

	int len;
	//注册：姓名(15)+性别(5)+账号(10)+密码(8)
	//（格式：REG-NAME:15-SEX:5-ID:10-PSW:8）
	char buf[1024] = {0};
	printf("REG:\n");

	char name[17] = {0};
	printf("Name(15):");
	fgets(name, sizeof(name), stdin);
	name[strlen(name) - 1] = 0;

	char sex[7] = {0};
	printf("Sex(5):");
	fgets(sex, sizeof(sex), stdin);
	sex[strlen(sex) - 1] = 0;

	char id[12] = {0};
	printf("Id(10):");
	fgets(id, sizeof(id), stdin);
	id[strlen(id) - 1]  = 0;

	char pwd[10] = {0};
	printf("Password(8):");
	fgets(pwd, sizeof(pwd), stdin);
	pwd[strlen(pwd) - 1] = 0;

	sprintf(buf, "REG-NAME:%s-SEX:%s-ID:%s-PSW:%s", name, sex, id, pwd);
	if(0 > send(sockfd, buf, sizeof(buf), 0)){
		perror("send REG order err to server!");
		return -1;
	}else{
		printf("send success!\n");
	}

	int ret;
	bzero(buf, sizeof(buf));
	ret = recv(sockfd, buf, sizeof(buf), 0);
	if(0 > ret){
		perror("recv reg status err from server");
		return -1;
	}
	printf("regist status: %s\n", buf);
	close(sockfd);
	return 0;

}

int server(int sockfd){

	char buf[1024] = {0};
	int ret;
	int op = 999;
	while(op){
		serverMenu();
		printf("select> ");
		op = 999;
		scanf("%d", &op);
		getchar();//去掉换行
		switch(op){
			//查询单词并返回结果
			case 1:   {	 
					  int len;
					  char word[20] = {0};
					  while(1){
						  bzero(buf, sizeof(buf));
						  printf("search: ");
						  fgets(word, sizeof(word), stdin);		
						  word[strlen(word) - 1] = 0;
						  sprintf(buf, "search: %s", word);
						  if(strcmp(buf + 8, "0000") == 0)
							  break;
						  if(0 > send(sockfd, buf, sizeof(buf), 0)){
							  perror("send order err to server while search!");
							  return -1;
						  }

						  bzero(buf, sizeof(buf));
						  ret = recv(sockfd, buf, sizeof(buf), 0);
						  if(0 > ret){
							  perror("recv from data err!");
							  return -1;
						  }
						  if(0 == ret){
							  printf("recv data from server over!\n");
							  break;
						  }	
						  printf("explain: %s", buf);

					  }
				  }; break;
			case 2:    {
					   bzero(buf, sizeof(buf));
					   sprintf(buf, "history");
					   if(0 > send(sockfd, buf, sizeof(buf), 0)){
						   perror("send order err to server while history!");
						   return -1;
					   }
					   bzero(buf, sizeof(buf));
					   ret = recv(sockfd, buf, sizeof(buf), 0);
					   if(0 > ret){
						   perror("recv from data err!");
						   return -1;
					   }
					   if(0 == ret){
						   printf("recv data from server over!\n");
					   }	
					   printf("history:\n %s\n", buf);

				   };break;//查看历史
			case 0:  close(sockfd); printf("Login out\n");break;
			default: printf("err opt!\n");break;
		}
	}
	return 0;
}


int main(int argc, char * argv[]){

	//检验命令行参数
	if(3 > argc){
		fprintf(stderr, "SRV:%s SRV_IP SRV_PORT", argv[0]);
		return -1;
	}

	//定义服务器网络地址结构体并填充
	SAI srvaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = inet_addr(argv[1])
	};

	//客户端显示菜单
	int op = 999;
	int sockfd;
	while(op){
		clientMenu();
		printf("select> ");
		scanf("%d", &op);
		getchar();
		switch(op){
			case 1:{
				       //创建套接字
				       sockfd = socket(AF_INET, SOCK_STREAM, 0);
				       if(0 > sockfd){
					       perror("socket");
					       return -1;
				       }
				       if(0 > connect_server(sockfd, srvaddr))
					       goto ERR;
				       if(0 > login(sockfd))
					       continue;
				       if(0 > server(sockfd))
					       goto ERR;


			       }/*连接客户端，登陆验证成功后进入服务器菜单界面 */ break;
			case 2:{
				       //创建套接字
				       sockfd = socket(AF_INET, SOCK_STREAM, 0);
				       if(0 > sockfd){
					       perror("socket");
					       return -1;
				       }
				       if(0 > connect_server(sockfd, srvaddr))
					       goto ERR;

				       if(0 > Reg(sockfd))
					       goto ERR;

			       };break;/*连接客户端，注册信息验证，存在就不注册，并提示相关信息，注册成功提示成功信息*/;
			case 0:printf("client exit\n");break;/*客户端退出*/
			default: printf("error opt!\n");break;
		}

	}

	return 0;
ERR:
	close(sockfd);
	return -1;
}
