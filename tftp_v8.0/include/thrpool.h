#ifndef _THRPOOL_H
#define _THRPOOL_H
#include "thr.h"

//线程池对象：线程对象 + 特定的描述符
typedef struct __thrpool {
	thr_t thr;
	int fd;
}thrpool_t;

//无效fd宏
#define INVALIDFD -1
//线程池能开启的最大线程数量
#define POOLMAX 15

//线程池(有特定用途的线程对象数组)
//外部引用声明
extern thrpool_t pool[POOLMAX];

//初始化线程池(所有的线程对象都开启线程并处于挂起状态)
void thrpool_init(PFUN pfun);
//得到空闲线程
int get_thr_id();

#endif
