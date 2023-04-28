#ifndef _NET_H
#define _NET_H
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX 1024//接收或者发送buf的大小
char buf[MAX];
int confd;//双方建立通信的套接字

typedef struct sockaddr_in SAI;//internet 地址结构体
typedef struct sockaddr SA;//通用地址结构体
SAI seraddr, cliaddr;//服务器端的网络地址，客户端的网络地址

#define err_log(log, ERR)\
	do{\
		perror(log);\
		goto ERR;\
	}while(0);
//socklen_t addrlen;//accept时需要指定客户端的长度

#define print(msg, pos)\
	do{\
		printf(msg);\
		goto pos;\
	}while(0);
#endif

