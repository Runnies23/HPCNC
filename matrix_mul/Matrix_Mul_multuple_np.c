#include <mpi.h>
#include <stdio.h>
#include <time.h>

#define N 500 //set Matrix size (Even Numbers)

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
	
	int matrix_1[N][N];
       	int matrix_2[N][N];
       	int matrix_rst[N][N];
	if (rank == 0){

		clock_t start = clock();

		//int matrix_1[ROWS][COLS] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
		//int matrix_2[ROWS][COLS] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
		//init the matrix_rst with 0
		
		int count = 0; 
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N;j++){
				matrix_1[i][j] = count;
				matrix_2[i][j] = count;
				count = count + 1;
				matrix_rst[i][j] = 0;
			}
		}
	
		//for (int i = 0; i < N; i++){
		//	for (int j = 0;j < N;j++){
		//		matrix_rst[i][j] = 0;
		//	}
		//}
	
		MPI_Send(&matrix_1[N/2][0], (N / 2) * N, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(&matrix_2[0][0], N * N, MPI_INT, 1, 0, MPI_COMM_WORLD);
		
		//Compute First Half matrix
		for (int i = 0; i < N/2; i++){
			for (int j = 0; j < N; j++){
				for (int k = 0; k < N; k++){
					matrix_rst[i][j] = matrix_rst[i][j] + (matrix_1[i][k] * matrix_2[k][j]);
				}		
			}
		}	

		//receive Second half matrix
		MPI_Recv(&matrix_rst[N/2][0], (N/2) * N, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		//print_matrix(matrix_rst);
		clock_t end = clock();
		double processing_time = (double)(end - start)/ CLOCKS_PER_SEC;
		printf("Processing time : %lf s", processing_time );
	}

	if (rank == 1){
		
		int local_A[N/2][N];
		MPI_Recv(&local_A, (N / 2) * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&matrix_2[0][0], N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		

		int local_C[N/2][N];
		//printf("This Is rank : %d", rank);
		
		//Compute the matrix
		for (int i = 0;i < N/2; i++){
			for (int j = 0; j < N; j++){
				local_C[i][j] = 0;
				for (int k = 0; k < N; k++){
					local_C[i][j] = local_C[i][j] + (local_A[i][k] * matrix_2[k][j]);
				}
			}
		}

		MPI_Send(&local_C[0][0], (N/2) * N, MPI_INT, 0, 0, MPI_COMM_WORLD);


	}
	MPI_Finalize();
	return 0 ;

}
