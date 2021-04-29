#include "libminiomp.h"

// This file implements the PARALLEL construct as part of your 
// miniomp runtime library. parallel.h contains definitions of 
// data types used here

// Declaration of array for storing pthread identifier from 
// pthread_create function
pthread_t *miniomp_threads;

// Global variable for parallel descriptor
miniomp_parallel_t *miniomp_parallel;

// Declaration of per-thread specific key
pthread_key_t miniomp_specifickey;

// This is the prototype for the Pthreads starting function
void *worker(void *args) {
	// Set the data
	miniomp_parallel_t *data = args;
	pthread_setspecific (miniomp_specifickey, data);
	// Execute the function with the determined data
	data->fn(data->fn_data);	
	// After execution,
	//   Sign task while the task queue is not empty
	while(!TQis_empty(miniomp_taskqueue)){
		miniomp_task_t *task = TQfirst(miniomp_taskqueue);
		TQdequeue(miniomp_taskqueue);
		if(task!=NULL)
			task->fn(task->data);
	}
	// Exit
	pthread_exit(NULL);
}

void GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads, unsigned int flags) {
	// Set default number of threads
	if(!num_threads) num_threads = omp_get_num_threads();
	// If we have the number of threads specified,
	// We update the explicit barriers
	else{
		miniomp_icv.nthreads_var = num_threads;
		pthread_barrier_init(&miniomp_barrier, NULL, num_threads);
	}
	
	int rc;
	// Requesting memory for all threads
	miniomp_threads = malloc(num_threads * sizeof(pthread_t));
	miniomp_parallel = malloc(num_threads * sizeof(miniomp_parallel_t));	
	// Creating Threads
	for (int i=0; i<num_threads; i++){
		// Initializing data for parallel descriptor of each thread
		miniomp_parallel[i].fn = fn;
		miniomp_parallel[i].fn_data = data;
		miniomp_parallel[i].id = i;
		// Create thread
		rc = pthread_create(&miniomp_threads[i], NULL, &worker, (void *) &miniomp_parallel[i]);
	  	if(rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}		
	}
	// Implicit barrier	
	for(int j=0; j< num_threads; j++)
		pthread_join(miniomp_threads[j], NULL);
}
