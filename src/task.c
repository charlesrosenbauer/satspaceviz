#include "stdint.h"
#include "stdlib.h"

#include "task.h"



ThreadPool makePool(int tct){
	ThreadPool ret;
	ret.threads = tct;
	ret.pool	= malloc(sizeof(pthread_t) * tct);
	ret.lock	= 0;
	ret.taskct	= 0;
	return ret;
}


void runTask(ThreadPool p, void* f(void*)){

}
