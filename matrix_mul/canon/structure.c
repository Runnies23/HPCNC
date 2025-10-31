#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h> 
#include <stdbool.h>

int main(int argc, char** argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);

	const int N = 5;
	clock_t start, end;

	int matrix_A[N][N], matrix_B[N][N];
	//create a matrix A and B 

	if (rank == 0){
		start = clock();

		int count = 1;
		for(int i = 0;i < N;i++){
			for(int j = 0;j < N;j++){
				matrix_A[i][j] = count;
				matrix_B[i][j] = count;
				count++;	
			}
		}
	}

	int np = size;
	float root_np_float = sqrt(np);
	int root_np = (int)root_np_float;

	int dims[2] = {root_np, root_np};
	int periods[2] = {true, true}; // periods [true, false]
	bool reorder = true;

	MPI_Comm Cartesian; // create a Cartesian Communication
	MPI_Cart_create(MPI_COMM_WORLD,
			root_np,
			dims,
			periods,
			reorder,
			&Cartesian);	
	
	int coords[root_np];
	MPI_Cart_coords(
			Cartesian,
			rank, 
			root_np,
			coords
			);// get x,y coordinate in grid virtual processor
	//printf("%p",Cartesian); //print to see Catesian
	
	int displs[4] = {0,4,8,12};
	int sendcount[4] = {4,4,4,4};

	int recv_count = sendcount[rank];
	int local_matrix_a[N];
	MPI_Scatterv(matrix_A,
			sendcount,
			displs,
			MPI_INT,	
			&local_matrix_a,
			recv_count,
			MPI_INT,
			0,
			Cartesian);

	if(rank == 0){
		printf("Rank : %d\n",rank);
		int lenght = (sizeof(coords) / sizeof(int));
		for (int i = 0;i < lenght;i++){	
			printf("idx : %d : coords : %d\n",i, coords[i]);
		}
		printf("\n");
	}

	//print debug 
	if(rank == 1){
		for (int i = 0;i < N;i++){
		//	for (int j = 0;j < N;j++){
				printf("%d ",local_matrix_a[i]);
		//	}
		//	printf("\n");
		}
		printf("\n");
	}	


	//trying to MPI_Cart_shift
	int left, right;
	MPI_Cart_shift(
			Cartesian,
			1, // if this is 1 will be columns [up, down]
			1,
			&left,
			&right 
			);

	if(rank == 0){
		printf("communicate on rows\n");
		printf("Rank : %d | left : %d | right : %d\n",rank, left, right);
	}

	if(rank == 0){
		for(int i = 0;i < N;i++){
		//	for(int j = 0;j < N;j++){
				printf("%d ",local_matrix_a[i]);
		//	}
		//printf("\n");
		}
	}	
	

	int check_rank = 1;
	
	if (rank == 0){
		end = clock();
		double processing_time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Processing Time : %lf s", processing_time);
	}

	MPI_Finalize();
	return 0 ;

}
