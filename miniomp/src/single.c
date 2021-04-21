#include "libminiomp.h"

// Declaratiuon of global variable for single work descriptor
miniomp_single_t miniomp_single;

// This routine is called when first encountering a SINGLE construct. 
// Returns true if this is the thread that should execute the clause. 

bool GOMP_single_start (void) {
	int id = omp_get_thread_num();
	if(miniomp_single.idThread==id)
		return (true);
	else if(__sync_bool_compare_and_swap(&miniomp_single.single, false, true)){
		miniomp_single.idThread = id;
		return (true);
	}	
	else
		return(false);
}
