#include <mpi.h>
#include <stdio.h>
#include <time.h>

#define N 500

int main(int argc, char** argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);
	
	if (rank == 0){
	clock_t start = clock();

	//const int ROWS = 3;
       	//const int COLS = 3;

	int matrix_1[N][N];// = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
	int matrix_2[N][N];// = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} }; 
	
	int matrix_rst[N][N];

	int count = 0;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			matrix_1[i][j] = count;
			matrix_2[i][j] = count;
			count = count + 1;
			matrix_rst[i][j] = 0;
		}
	}

	for (int i = 0; i < N;i++){
		for (int j = 0; j < N;j++){
			
			//matrix_rst[i][j] = 0;
			for (int k = 0; k < N; k++){
				matrix_rst[i][j] = matrix_rst[i][j] + (matrix_1[i][k] * matrix_2[k][j]);

			}
		}
	}	

	//for (int i = 0; i < N; i++){
	//	for (int j = 0; j < N; j++){
	//		printf("\t%d",matrix_rst[i][j]);
	//	}
	//	printf("\n");
	//}

	clock_t end = clock();

	double processing_time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("Processing Time : %lf s", processing_time);
	
	}

	MPI_Finalize();
	return 0 ;


}
