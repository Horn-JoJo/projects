#ifndef _LOCK_H
#define _LOCK_H

#include "net.h"
#include<time.h>
#include<fcntl.h>
//#include<signal.h>
#include<sqlite3.h>

//unsigned char key;//默认是0
size_t sizes;//默认是0

//void hander(int);
static void encode(char [], int, unsigned char);
static void reverse(char *);
static int deal(char *, char *);
//开放接口
void do_lock();

#endif
