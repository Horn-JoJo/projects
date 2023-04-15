#include "thr.h"

static void *CALLBACK(void *arg)
{
	thr_t *obj = (thr_t *)arg;
	int oldstate, oldtype;

	pthread_setcancelstate(PTHRAD_CANCEL_ENABLE, &oldstate);
	pthread_setcanceltype(PTHRAD_CANCEL_DEFERRED, &oldstype);

	pthread_mutex_init(&this->mutex, NULL);
	pthread_cond_init(&this->cond, NULL);

	this->isAlive = 1;//表明线程对象已经在运行线程了

	while (1)
	{
		//为什么需要保护？因为isSusend可以会再其他地方被修改。就可能造成isSusend数据会混乱
		pthread_mutex_lock(&this->mutex);
		if (this->isSusend) 
			pthread_cond_wait(&this->cond, &this->mutex);
		pthread_mutex_unlock(&this->mutex);

		this->pfun(this->arg);//回调注册的工作函数
	}
	return (void *)0;
}

void thr_init(thr_t *obj, PFUN pfun, void *arg)
{
	obj->pfun     = pfun;
	obj->arg  	  = arg;
	obj->isAlive  = 0;
	obj->isSusend = 1;
}

int thr_destroy(thr_t *obj)
{
	if (obj->isAlive)
	{
		pthread_cancel(obj->tid);
		pthread_mutex_destroy(&obj->mutex);
		pthread_cond_destroy(&obj->cond);
		return 0;
	}
	return -1;
}

int thr_create(thr_t *obj) 
{
	if (!obj->isAlive && obj->pfun) 
		return pthread_create(&obj->tid, NULL, CALLBACK, (void *)obj);
	return -1;
}

void thr_resume(thr_t *obj) 
{
	if (obj->isAlive)
	{
		if (obj->isSusend) 
		{
			pthread_mutex_lock(&obj->mutex);
			obj->isSusend = 0;
			pthread_mutex_unlock(&obj->mutex);
			
			pthread_cond_signal(&obj->cond);
		}
	}
}

void thr_susend(tht_t *obj)
{
	if (obj->isAlive)
	{
		if (!obj->isSusend)
		{
			pthread_mutex_lock(&obj->mutex);
			obj->isSusend = 1;
			pthread_mutex_unlock(&obj->mutex);
		}
	}
}

int thr_alive(thr_t *obj)
{
	return obj->isAlive;
}
