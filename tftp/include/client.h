#ifndef _CLIENT_H
#define _CLIENT_H
#include "net.h"
#include "lock.h"

#define SIZE 128
void do_get(char []);//从服务器端下载文件
void do_put(char []);//上传文件到服务器端
void do_list(char []);//显示服务器给定客户端的目录内容
void do_help();//离线帮助手册
//已经包含在lock文件中
//void do_lock();//实现加锁和解锁解锁指定的文件！！！

//暂时先不考虑实现如下两个操作
void do_log_in();//实现登录操作
void do_sign_up();//实现注册操作



//下面的这个err_log可以优化成更加通用的形式！！！
/*#define err_log(log)\
	do{\
		perror(log);\
		return;\
	}while(0);*/
#endif

