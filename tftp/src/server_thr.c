#include "thr.h"
#include <stdio.h>

//线程函数
static void *CALLBACK(void *arg)
{
	printf("thread fun: %s is running...\n", __func__);
	thr_t *this = (thr_t *)arg;
	int oldstate, oldtype;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);

	pthread_mutex_init(&(this->mutex), NULL);
	pthread_cond_init(&(this->cond), NULL);

	this->isAlive = 1;//表明线程对象已经创建出了线程

	while (1)
	{
		//为什么需要保护？因为isSusend可以会再其他地方被修改。就可能造成isSusend数据会混乱
		pthread_mutex_lock(&(this->mutex));
		if (this->isSusend) 
			pthread_cond_wait(&(this->cond), &(this->mutex));
		pthread_mutex_unlock(&(this->mutex));

		//循环回调
		this->pfun(this->arg);//回调注册的工作函数
	}
	return (void *)0;
}

//初始化线程对象
void thr_init(thr_t *obj, PFUN pfun, void *arg)
{
	obj->pfun     = pfun;
	obj->arg  	  = arg;
	obj->isAlive  = 0;
	obj->isSusend = 1;
}

//销毁线程对象
int thr_destroy(thr_t *obj)
{
	if (obj->isAlive)
	{
		pthread_cancel(obj->tid);
		pthread_mutex_destroy(&(obj->mutex));
		pthread_cond_destroy(&(obj->cond));
		return 0;
	}
	return -1;
}

//线程对象创建一个线程：用于回调自己的工作函数
//不同线程对象重载同一个线程函数回调不一样的工作函数
int thr_create(thr_t *obj) 
{
	if (!obj->isAlive && obj->pfun) 
		return pthread_create(&obj->tid, NULL, CALLBACK, (void *)obj);
	return -1;
}

//唤醒线程
void thr_resume(thr_t *obj) 
{
	if (obj->isAlive)
	{
		if (obj->isSusend) 
		{
			pthread_mutex_lock(&(obj->mutex));
			obj->isSusend = 0;
			pthread_mutex_unlock(&(obj->mutex));
			
			pthread_cond_signal(&(obj->cond));
		}
	}
}

//挂起线程
void thr_susend(thr_t *obj)
{
	if (obj->isAlive)
	{
		if (!obj->isSusend)
		{
			pthread_mutex_lock(&(obj->mutex));
			obj->isSusend = 1;
			pthread_mutex_unlock(&(obj->mutex));
		}
	}
}

//判断线程对象是否开启了线程
int thr_alive(thr_t *obj)
{
	return obj->isAlive;
}
