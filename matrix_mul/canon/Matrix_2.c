#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);

	
	clock_t start, end;

	if (rank == 0){
		//init the start program check point clock() for compute times
		start = clock();
	}

	// init relate valueable
	const int N = 10;
	const int np = size;

	//global (rank 0)
	int matrix_A[N][N]; // Matrix A in each processor 
	int matrix_B[N][N]; // Matrix B in each processor
	// if rank 0 size N * N but another is N / P * N


	//local value (each np)
	int size_root = sqrt(np);
	int data_per_np = (int)(N / size_root);
	int rows_per_np = N / np;
	int local_A[data_per_np][N];
	int local_B[data_per_np][N];
	int local_result[data_per_np][data_per_np];
	if (rank == 0){
		printf("Nums Processor : %d\n", size);
		printf("Nums np root : %d\n",size_root);
		printf("data per np : %d\n", data_per_np);
	}


	if (rank == 0){
		int count = 1;
		//create a matrix_A and Matrix_B in rank 0  
		for (int i = 0; i < N; i++){
			for (int j = 0;j < N; j++){
				matrix_A[i][j] = count;
				//matrix_B[i][j] = count;
				//transpose the matrix B
				matrix_B[j][i] = count;
				
				//printf("%d  ",count);
				count++;
			
			}
			//printf("\n");
		}
	}


	if (rank == 0){
		for (int i = 0;i < N;i++){
			for(int j = 0;j < N;j++){
				printf("%d ", matrix_A[i][j]);
			}
			printf("\n");
		}
	printf("\n");
	}

	//assume have 4 processor (2x2) in 4x4 matrix
	//int count[np] = {8, 8, 8, 8};
	//int displs[np] = {0, 0, 8, 8};
	//int recv_count = count[rank];
	//int displs_b[np] = {0, 8, 0, 8};

	int count[np], displs[np], displs_b[np];
	
	int comm_data_size = N * data_per_np;
	int step_a = 0;
	int step_b = 0;
	int count_step_a = 0;
	int count_step_b = 0;
	for (int i = 0;i < np;i++){
		count[i] = comm_data_size;
		displs[i] = step_a;
		displs_b[i] = step_b;
		//condition of displs A 
		if (count_step_a == size_root - 1){
			step_a += comm_data_size;
			count_step_a = 0;
		}else{
			count_step_a++;
		}

		//condition of displs B
		if (count_step_b == size_root - 1){
			count_step_b = 0;
			step_b = 0;
			continue;
			//step_b = step_b + comm_data_size;
		}else{
			//step_b = step_b + comm_data_size;
			count_step_b++;
		}
		step_b = step_b + comm_data_size;
	}
	int recv_count = count[rank];

	if (rank == 0){
		//printf("Data per np : %d\n", data_per_np);
		printf("displs_A\n");
		for(int i = 0;i < np;i++){
			printf("%d ",displs[i]);
		}
		printf("\n");

		printf("displs_B\n");
		for(int i = 0;i < np;i++){
			printf("%d ",displs_b[i]);
		}
		printf("\n");
		
	
	printf("=======\n");
	}

	// Scatter all the data to all processor both Matrix A and Matrix B
	MPI_Scatterv(matrix_A, count, displs, MPI_INT, local_A, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(matrix_B, count, displs_b, MPI_INT, local_B, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);

	int check_rank = 4;
	if (rank == check_rank){
		printf("Rank : %d\n",rank);
		for (int i = 0;i < 2;i++){
			for(int j = 0;j < N;j++){
				printf("%d ",local_A[i][j]);
			}
			printf("\n");
		}
		for (int i = 0;i < 2;i++){
			for(int j = 0;j < N;j++){
				printf("%d ",local_B[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}


	//compute in every processor
	for (int i = 0; i < data_per_np; i++){
		for (int j = 0;j < data_per_np; j++){
			local_result[i][j] = 0;
			for (int k = 0; k < N; k++){	
				local_result[i][j] += local_A[i][k] * local_B[j][k];	
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == check_rank){
		for(int i = 0;i < data_per_np; i++){
			for(int j = 0;j < data_per_np;j++){
				printf("%d ",local_result[i][j]);
			}
		printf("\n");
		}
	}


	MPI_Barrier(MPI_COMM_WORLD);
	//init data for receive result
	int matrix_result[N][N];

	//Gather all the result matrix
	MPI_Gather(local_result, data_per_np * data_per_np, MPI_INT, &matrix_result, data_per_np * data_per_np, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0){
		for( int i = 0; i < N; i++){
			for(int j = 0;j < N; j++){
				printf("%d  ",matrix_result[i][j]);
			}
		printf("\n");
		}
	}

	if (rank == 0){
		end = clock();

		double processing_time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Processing Time : %lf s",processing_time);
	}
	
	MPI_Finalize();
	return 0 ;

}
