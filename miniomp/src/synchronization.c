#include "libminiomp.h"

// Default lock for unnamed critical sections
pthread_mutex_t miniomp_default_lock;

void 
GOMP_critical_start (void) {
	// Locking the mutex
	pthread_mutex_lock(&miniomp_default_lock);
//	printf("TBI: Entering an unnamed critical, don't know if anyone else is alrady in. I proceed\n");
}

void 
GOMP_critical_end (void) {
	// Unlocking the mutex
	pthread_mutex_unlock(&miniomp_default_lock);
//	printf("TBI: Exiting an unnamed critical section. I can not inform anyone else, bye!\n");
}

void 
GOMP_critical_name_start (void **pptr) {
	pthread_mutex_t * plock = *pptr;
//	printf("TBI: Entering a named critical %p (%p), don't know if anyone else is alrady in. I proceed\n", pptr, plock);
  
	// if plock is NULL it means that the lock associated to the name has not yet been allocated and initialized
	if( plock == NULL){
  		plock = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(plock, NULL);
		*pptr = plock;
	}
	// Locking the section name 'pptr'
	pthread_mutex_lock(plock);
}

void 
GOMP_critical_name_end (void **pptr) {
	pthread_mutex_t * plock = *pptr;
//	printf("TBI: Exiting a named critical %p (%p), I can not inform anyone else, bye!\n", pptr, plock);

	// Unlocking the section name 'pptr'
	// if plock is still NULL something went wrong
	if ( plock != NULL) pthread_mutex_unlock (plock);
	else printf("ERROR: plock null, GOMP_critical_name_end, name %p\n", pptr);
}

// Default barrier within a parallel region
pthread_barrier_t miniomp_barrier;

void 
GOMP_barrier() {
	// Lock the thread waiting the execution of the others threads	
	pthread_barrier_wait(&miniomp_barrier);
//	printf("TBI: Entering in barrier, but do not know how to wait for the rest. I proceed\n");
}
