#ifndef _SERVAR_H
#define _SERVAR_H
#include "net.h"
#include "ls.h"
#include <pthread.h>
//#include <poll.h>
#include <sys/epoll.h>

#define PATH "../res/"
//char PATH[MAX] =  "../res/";
//struct pollfd fds[MAX];//MAX -net.h头文件中已经包含了
socklen_t addrlen;

void list_files();//以ls 的形式列出给定目录下的文件信息以及大小等
void get_file(char []);//登录状态下，从服务器客户端所在资源路径下载对应的文件到客户端指定的目录
void put_file();//登录状态下，将客户端指定路径的文件上传到服务器端客户端所在资源路径
void log_in();//登录，用户输入密码和账号，登录成功后可以实现上述的操作
void sign_up();//注册，如果不存在用户，则不能享受该服务


#endif
