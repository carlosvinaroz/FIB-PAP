#include <pthread.h>

extern bool miniomp_taskgroup;
extern pthread_barrier_t miniomp_barrier_taskwait;
extern pthread_barrier_t miniomp_barrier_taskgroup;

// Functions implemented in this module
void GOMP_taskwait(void);
void GOMP_taskgroup_start (void);
void GOMP_taskgroup_end (void);
