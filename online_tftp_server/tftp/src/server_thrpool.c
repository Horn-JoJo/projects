#include "thrpool.h"

//线程池对象{线程对象 + 描述符fd}
thrpool_t pool[POOLMAX];

//开启POLLMAX个线程，开始都处于挂起状态(并未回调线程工作函数)
void thrpool_init(PFUN pfun)
{
	for (int i = 0; i < POOLMAX; i++)
	{
		pool[i].fd = INVALIDFD;
		thr_init(&pool[i].thr, pfun, (void *)&pool[i]);
		thr_create(&pool[i].thr);
	}
}

//获取空闲的线程池对象(对应的线程池对象开启的线程是处于挂起状态)
int get_thr_id()
{
	for (int i = 0; i < POOLMAX; i++)
	{
		if (pool[i].fd == INVALIDFD)//套接字无效就表明线程池对象开启的线程是没有活干
			return i;
	}
	return -1;
}
