#include "thrpool.h"

thrpool_t pool[POLLMAX];

//开启POLLMAX个线程，开始都处于挂起状态(并未回调线程工作函数)
void thrpool_init(PFUN pfun)
{
	for (int i = 0; i < POLLMAX; i++)
	{
		pool[i].fd = INVALIDFD;
		thr_init(&pool[i].obj, pfun, (void *)&pool[i]);
		thr_create(&pool[i].thr);
	}
}

//获取
int get_thr_idx()
{
	for (int i = 0; i < POLLMAX; i++)
	{
		if (poll[i].fd == INVALIDFD)
			return i;
	}
	return -1;
}
