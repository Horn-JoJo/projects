#ifndef _CLIENT_H
#define _CLIENT_H
#include "net.h"

void do_get(char []);//从服务器端下载文件
void do_put(char []);//上传文件到服务器端
void do_list(char []);//显示服务器给定客户端的目录内容
void do_help();//离线帮助手册
void do_log_in();//实现登录操作
void do_sign_up();//实现注册操作

#endif

