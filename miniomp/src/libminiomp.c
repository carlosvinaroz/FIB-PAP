#include "libminiomp.h"

// Library constructor and desctructor
void init_miniomp(void) __attribute__((constructor));
void fini_miniomp(void) __attribute__((destructor));

void init_miniomp(void) {
	printf ("mini-omp is being initialized\n");
	// Parse OMP_NUM_THREADS environment variable to initialize nthreads_var internal control variable
	parse_env();
	// Initialize pthread and parallel data structures 
	miniomp_threads = malloc(MAX_THREADS * sizeof(pthread_t));
	miniomp_parallel = malloc(MAX_THREADS * sizeof(miniomp_parallel_t));
	// Initialize Pthread thread-specific data, now just used to store the OpenMP thread identifier
	pthread_key_create(&miniomp_specifickey, NULL);
	pthread_setspecific(miniomp_specifickey, (void *) 0); // implicit initial pthread with id=0
	// Initialize OpenMP default locks and default barrier
	pthread_mutex_init(& miniomp_default_lock,NULL);
	pthread_barrier_init(&miniomp_barrier, NULL, omp_get_num_threads());
	// Initialize OpenMP workdescriptors for single 
	miniomp_single.single=false;
	miniomp_single.idThread=-1;
	// Initialize OpenMP task queue for task and taskloop
	miniomp_taskqueue=TQinit(MAXELEMENTS_TQ);
	miniomp_taskgroup=false;
	pthread_barrier_init(&miniomp_barrier_taskwait, NULL, 1);
}

void fini_miniomp(void) {
	// delete Pthread thread-specific data
	pthread_key_delete(miniomp_specifickey);
	// free other data structures allocated during library initialization
	pthread_mutex_destroy(&miniomp_default_lock);
	pthread_barrier_destroy(&miniomp_barrier);
	pthread_barrier_destroy(&miniomp_barrier_taskwait);
	free(miniomp_threads);
	free(miniomp_parallel);
	printf ("mini-omp is finalized\n");
}
