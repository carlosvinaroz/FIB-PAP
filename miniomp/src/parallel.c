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
	miniomp_parallel_t *data = args;
	//printf("Worker: Thread %d\n", data->id);
	// insert all necessary code here for:
	//   1) save thread-specific data (id)
	pthread_setspecific (miniomp_specifickey, data);
	//   2) invoke the per-threads instance of function encapsulating the parallel region
	data->fn(data->fn_data);
	//   3) exit the function
	pthread_exit(NULL);
}

void GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads, unsigned int flags) {
	if(!num_threads) num_threads = omp_get_num_threads();
	else{
		miniomp_icv.nthreads_var = num_threads;
		pthread_barrier_init(&miniomp_barrier, NULL, num_threads);
	}
	//printf("Starting a parallel region using %d threads\n", num_threads);
	int rc;
	// Requesting memory for all threads
	miniomp_threads = malloc(num_threads * sizeof(pthread_t));
	miniomp_parallel = malloc(num_threads * sizeof(miniomp_parallel_t));	
	// Creating Threads (num_threads)
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
		//printf("Thread %d created\n", i);
	}
	// Implicit barrier	
	for(int j=0; j< num_threads; j++)
		pthread_join(miniomp_threads[j], NULL);
	}
