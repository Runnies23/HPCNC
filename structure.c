#include <mpi.h>
#include <stdio.h>

#include <stdlib.h>
#include

int main(int argc, **char argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);
	
	printf("Hello World from processor cpu-clx-%s.hpcnc.in.th, rank%d out of 4 processor \n", processor_name,rank);
	
	MPI_Finalize();
	return 0 
	
}


