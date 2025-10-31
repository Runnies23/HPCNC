#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define N 1000 //set Matrix size (Even Numbers)

void print_matrix(int matrix[N][N]){

	for (int i = 0; i < N; i ++){
		for (int j = 0; j < N; j++){
			printf("\t%d",matrix[i][j]);
		}
		printf("\n");
	}
	
	return;
}

int main(int argc, char** argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);
	
	//int matrix_1[N][N];
       	//int matrix_2[N][N];
	clock_t start, end;
	if(rank == 0){
		start = clock();
	}
	
	int *matrix_1 = NULL;
	int *matrix_2 = (int*)malloc(sizeof(int) * N * N);

	//if rank 0 init the matrix and Scatter(Matrix1) and Bcast(Matrix2) to all the processor
	// if N % size != 0 that is error cuz it's remain the rows that not compute - send
	if (N % size != 0){
		MPI_Abort(MPI_COMM_WORLD,1);
		return(1);
	}

	if (rank == 0){
		
		matrix_1 = (int *)malloc(sizeof(int) * N * N);
		//matrix_2 = (int *)malloc(sizeof(int) * N * N);

		//printf("Rank : %d | Size : %d\n",rank, size);
		int count = 0;
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N;j++){
				matrix_1[i * N + j] = count;
				matrix_2[i * N + j] = count;
				count = count + 1;
			}
		}
	
	}
	
	// send all the matrix 2 to all processor
	MPI_Barrier(MPI_COMM_WORLD); // wait all the compute ready (create a matrix data)
	MPI_Bcast(matrix_2, N * N, MPI_INT, 0, MPI_COMM_WORLD);
	
	int rows_per_np = N / size;
	int size_per_np = (N * N) / size;
	int *local_matrix = (int *)malloc(sizeof(int) * size_per_np);
	int *local_result_matrix = (int *)malloc(sizeof(int) * size_per_np);

	// send all the data to all processor
	MPI_Scatter(matrix_1, size_per_np, MPI_INT, local_matrix, size_per_np, MPI_INT, 0, MPI_COMM_WORLD);
	
	//compute 
	for (int i = 0; i < rows_per_np; i++){
		for (int j = 0; j < N; j++){
			local_result_matrix[i * N + j] = 0;
			for (int k = 0; k < N; k++){
				local_result_matrix[i * N + j] += local_matrix[i * N + k] * matrix_2[k * N + j];
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	// gather all the processor
	int *matrix_rst = NULL;
	if (rank == 0){
	 	matrix_rst = (int *)malloc(sizeof(int) * N * N);
	}
	

	MPI_Gather(local_result_matrix, size_per_np, MPI_INT, matrix_rst, size_per_np, MPI_INT, 0, MPI_COMM_WORLD);
	free(local_matrix);
	free(local_result_matrix);

	// print result
	if (rank == 0){
		//for (int i = 0; i < N; i++){
		//	for (int j = 0;j < N; j++){
		//		printf("%d ", matrix_rst[i * N + j]);
		//	}
		//	printf("\n");
		//}
		free(matrix_rst);
		free(matrix_1);
	}
	free(matrix_2);


	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0){
		end = clock();
		double processing_time = (double)(end - start) / CLOCKS_PER_SEC; 
		printf("Time : %lf s", processing_time);
	}
	
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
