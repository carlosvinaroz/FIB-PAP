#include "libminiomp.h"

// Default lock for unnamed critical sections
pthread_mutex_t miniomp_default_lock;

void GOMP_critical_start (void) {
	// Locking the mutex
	pthread_mutex_lock(&miniomp_default_lock);
}

void GOMP_critical_end (void) {
	// Unlocking the mutex
	pthread_mutex_unlock(&miniomp_default_lock);
}

void GOMP_critical_name_start (void **pptr) {
	pthread_mutex_t * plock = *pptr; 
	// If plock is NULL it means the lock associated to the name
	// has not yet been allocated and initialized
	if( plock == NULL){
  		plock = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(plock, NULL);
		*pptr = plock;
	}
	// Locking the section name 'pptr'
	pthread_mutex_lock(plock);
}

void GOMP_critical_name_end (void **pptr) {
	pthread_mutex_t * plock = *pptr;
	// Unlocking the section name 'pptr'
	if ( plock != NULL) pthread_mutex_unlock (plock);
	// If plock is still NULL something went wrong
	else printf("ERROR: plock null, GOMP_critical_name_end, name %p\n", pptr);
}

// Default barrier within a parallel region
pthread_barrier_t miniomp_barrier;

void GOMP_barrier() {
	// While the task queue is not empty,
	// Execute the next task
	while(!TQis_empty(miniomp_taskqueue)){
		miniomp_task_t *task = TQfirst(miniomp_taskqueue);
		TQdequeue(miniomp_taskqueue);
		if(task!=NULL)
			task->fn(task->data);
	}
	// Lock the thread waiting the execution of the others threads	
	pthread_barrier_wait(&miniomp_barrier);
}
