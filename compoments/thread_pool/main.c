#include <stdio.h>
#include <unistd.h>

//线程对象的头文件
#include "thr.h"
#define CNT(a) (sizeof(a) / sizeof (a[0]))

void *worker1(void *arg) {
	printf("fun:%s is running...\n", __func__);
	int num = (int)arg;
	printf("num = %d\n", num);
	sleep(1);
}

void *worker2(void *arg) {
	printf("fun:%s is running...\n", __func__);
	char *str = (char*)arg;
	printf("str = %s\n", str);
	sleep(1);
}

int main() {
	
	//测试线程的唤醒和挂起
	//1.定义线程对象数组：用来开启两个线程，分别处理不同的工作
	thr_t thr[2];
	
	//2.初始化线程对象
	thr_init(&thr[0], worker1, (void *)999);
	thr_init(&thr[1], worker2, "hello world!!!");

	//3.创建线程(开始都处于挂起状态，不会主动回调工作函数)
	thr_create(&thr[0]);
	thr_create(&thr[1]);
	
	
	while (1) {
		char ch = getchar();
		getchar();//吸收垃圾字符
		if ('r' == ch) {
			thr_resume(&thr[0]);
		} else if ('R' == ch) {
			thr_resume(&thr[1]);
		} else if ('s' == ch) {
			thr_susend(&thr[0]);
		} else if ('S' == ch) {
			thr_susend(&thr[1]);
		} else if ('q' == ch || 'Q' == ch) {
			break;
		}
	}

	return 0;
}
