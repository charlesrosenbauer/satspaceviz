#ifndef __TASK_HEADER__
#define __TASK_HEADER__


#include <pthread.h>




typedef struct{
	pthread_t*	pool;
	int			threads, lock;
	
	int			tasks[256];
	int			taskct;
}ThreadPool;


ThreadPool	makePool(int);
void		runTask	(ThreadPool, void*(void*));




#endif
