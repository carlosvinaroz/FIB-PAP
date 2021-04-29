#include "libminiomp.h"
#include <stdlib.h>

// Declare structures of taskwait and taskgroup
bool miniomp_taskgroup;
pthread_barrier_t miniomp_barrier_taskgroup;
pthread_barrier_t miniomp_barrier_taskwait;
// Called when encountering taskwait and taskgroup constructs
void GOMP_taskwait (void)
{
	// While the task queue is not empty, execute the next task
	while(!TQis_empty(miniomp_taskqueue)){
		miniomp_task_t *task = TQfirst(miniomp_taskqueue);
        	TQdequeue(miniomp_taskqueue);
		if(task!=NULL)
			task->fn(task->data);
	}	
	// Waiting all tasks
	pthread_barrier_wait(&miniomp_barrier_taskwait);
	// All tasks finished
}

void GOMP_taskgroup_start (void)
{
	// Activate taskgroup
	miniomp_taskgroup=true;
	pthread_barrier_init(&miniomp_barrier_taskgroup,NULL,1);
}

void GOMP_taskgroup_end (void)
{
	// Desactivate taskgroup
	miniomp_taskgroup=false;
	// While the task queue is not empty, execute the next task
	while(miniomp_taskqueue->taskgroupCount>0){
		miniomp_task_t *task = TQfirst(miniomp_taskqueue);
		TQdequeue(miniomp_taskqueue);
		if(task!=NULL)
			task->fn(task->data);
	}
	// Waiting all tasks
	pthread_barrier_wait(&miniomp_barrier_taskgroup);
	// All tasks finished
	// Destroy the barrier object
	pthread_barrier_destroy(&miniomp_barrier_taskgroup);
}
