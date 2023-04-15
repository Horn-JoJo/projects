#ifndef _THR_H
#define _THR_H
#include <pthread.h>

//线程对象需要哪些成员？
//1.工作函数 , 因此需要一个函数指针void *(*pfun)(void *);
//2.传给工作函数的参数 void *arg
//3.线程id
//4.需要表征该对象是否已经创建出了线程
//5.需要判断创建出的线程的运行状态
//6.唤醒和挂起该线程需要条件变量和互斥锁？？？

//定义一个void* (void*)类型的指针变量类型
typedef void * (*PFUN)(void *);

typedef struct {
	PFUN pfun;
	void *arg;
	pthread_t tid;
	isAlive;
	isSusend;
	ptread_mutex_t mutex;
	pthread_cond_t cond;
}thr_t;

//需要定义哪些方法？
//初始化线程对象
//销毁线程对象
//线程对象创建线程
//唤醒线程对象
//挂起线程对象
//判断线程对象是否在运行线程了

void thr_init(thr_t *, PFUN, void *);
int thr_destroy(thr_t *);
int thr_create(thr_t *);
void thr_resume(thr_t *);
void thr_susend(thr_t *);
int thr_alive(thr_t);


#endif
