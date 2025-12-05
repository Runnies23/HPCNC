#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define N 4

int main(int argc, char** argv){
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	// MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);

	clock_t start, end;
    
    bool debug = false;

	int size_root = sqrt(size);
    int N_each = N / size_root;
	const int np = size;
    const int check_rank = 0;
    const int main_rank = 0;

    int pivot = 0;

    int matrix_A[N][N];
    int matrix_B[N][N];
    int shift_matrix_A[N][N];
    int shift_matrix_B[N][N];

    int local_A[N_each][N_each];
    int local_B[N_each][N_each];
    int local_result[N_each][N_each]; 
    int result_all[N][N];

    int local_A_dimensions_full_array[2] = { N_each, N_each };
    int local_B_dimensions_full_array[2] = { N_each, N_each };
    int local_A_dimensions_subarray[2] = { N_each,1 };
    int local_B_dimensions_subarray[2] = { 1,N_each };
    
    int dimensions_full_array[2] = { N, N };
    int dimensions_subarray[2] = { N_each, N_each };

    //create a grid processor with 
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    int periods[2] = {true, true};
    int reorder = true;
    MPI_Comm grid_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid_communicator);
    enum DIRECTIONS {DOWN, UP, LEFT, RIGHT};
    char* neighbours_names[4] = {"down", "up", "left", "right"};
    int neighbours_ranks[4];
    MPI_Cart_shift(grid_communicator, 0, 1, &neighbours_ranks[LEFT], &neighbours_ranks[RIGHT]);
    MPI_Cart_shift(grid_communicator, 1, 1, &neighbours_ranks[DOWN], &neighbours_ranks[UP]);
    MPI_Comm_rank(grid_communicator, &rank);

    if (rank == 0){
		start = clock();
	}

    //create a main matrix
	if (rank == main_rank){
        if (debug){
            printf("np : %d\n", size);
            printf("np per rows : %d \n", size_root);
            printf("N Each : %d \n", N_each); 
        }
        //create a matrix
		for( int i = 0; i < N; i++){
			for(int j = 0;j < N; j++){
                matrix_A[i][j] = (i * N) + j + 1;
                matrix_B[i][j] = (i * N) + j + 1;
            }
		}
        if (debug){
            printf("=== Matrix A ===\n");
            for( int i = 0; i < N; i++){
                for(int j = 0;j < N; j++){
                    printf("%d ",matrix_A[i][j]);
                }
                printf("\n");
            }
            printf("=== Matrix B ===\n");
            for( int i = 0; i < N; i++){
                for(int j = 0;j < N; j++){
                    printf("%d ",matrix_B[i][j]);
                }
                printf("\n");
            }
            printf("\n");
        }

        //shift matrix
        int major = 0;
        for( int i = 0; i < N; i++){
            for(int j = 0;j < N; j++){
                shift_matrix_A[i][j] = matrix_A[i][(j + major)%N];
            }
            major++;
        }

        for(int j = 0;j < N; j++){
            for( int i = 0; i < N; i++){
                shift_matrix_B[i][j] = matrix_B[(i + major)%N][j];
            }
            major++;
        }

        if (debug){
            printf("Shift Matrix : \n");

            printf("=== Matrix A ===\n");
            for( int i = 0; i < N; i++){
                for(int j = 0;j < N; j++){
                    printf("%d ",shift_matrix_A[i][j]);
                }
                printf("\n");
            }
            printf("=== Matrix B ===\n");
            for( int i = 0; i < N; i++){
                for(int j = 0;j < N; j++){
                    printf("%d ",shift_matrix_B[i][j]);
                }
                printf("\n");
            }
            printf("\n");
        }
	}

    if (debug){
        if(rank == main_rank){
            printf("create a grid processor \n");
        }
    }


    if (debug){
        if(rank == main_rank){
            printf("Scatter matrix to all processer \n");
        }
    }

    //scatter all main matrix to grid sub array
    if (rank == main_rank){
        MPI_Datatype subarray_type;
        for (int i = 0;i < size;i++){
            int start_coordinates[2] = { ((int)(i / size_root) * N_each) , ((i % size_root) * N_each) };
            MPI_Type_create_subarray(2,  dimensions_full_array, dimensions_subarray, start_coordinates, MPI_ORDER_C, MPI_INT, &subarray_type);
            MPI_Type_commit(&subarray_type);

            MPI_Send(shift_matrix_A, 1, subarray_type, i, 0, grid_communicator);
            MPI_Send(shift_matrix_B, 1, subarray_type, i, 0, grid_communicator);
        }
    }

    MPI_Recv(&local_A, N_each * N_each, MPI_INT, 0, 0, grid_communicator, MPI_STATUS_IGNORE);
    MPI_Recv(&local_B, N_each * N_each, MPI_INT, 0, 0, grid_communicator, MPI_STATUS_IGNORE);
    MPI_Barrier(grid_communicator);

    if (debug){
        if (rank == check_rank){
            printf("Local A \n");
            for (int i = 0;i < N_each;i++){
                for (int j = 0;j < N_each;j++){
                    printf("%d ",local_A[i][j]);
                }
                printf("\n");
            }

            printf("Local B \n");
            for (int i = 0;i < N_each;i++){
                for (int j = 0;j < N_each;j++){
                    printf("%d ",local_B[i][j]);
                }
                printf("\n");
            }
            printf("Start =>\n");
        }
    }

    //init result matrix
    for (int i = 0;i < N_each;i++){
        for (int j = 0;j < N_each;j++){
            local_result[i][j] = 0;
        }
    }

    //compute 
    for (int i = 0;i < N;i++){
        
        if (debug){
            if (rank == check_rank){
                printf("P]ivot : %d\n",pivot);
                printf("Local A \n");
                for (int i = 0;i < N_each;i++){
                    for (int j = 0;j < N_each;j++){
                        printf("%d ",local_A[i][j]);
                    }
                    printf("\n");
                }

                printf("Local B \n");
                for (int i = 0;i < N_each;i++){
                    for (int j = 0;j < N_each;j++){
                        printf("%d ",local_B[i][j]);
                    }
                    printf("\n");
                }
            }
        }
        //compute 
        for (int i = 0;i < N_each;i++){
            for (int j = 0;j < N_each;j++){
                // for (int k = 0;k < N_each;k++){
                    // local_result[i][j] = local_result[i][j] + (local_A[i][j] * local_B[i][j]); 
                    local_result[i][j] += local_A[i][(j + pivot) % N_each] * local_B[(i + pivot) % N_each][j];
                // }
            }
        }

        MPI_Datatype local_A_subarray_type;
        MPI_Datatype local_B_subarray_type;
        
        int local_A_start_coordinates[2] = { 0,pivot };
        int local_B_start_coordinates[2] = { pivot,0 };
        
        // shift left matrix A (shitf down)
        MPI_Type_create_subarray(2,  local_A_dimensions_full_array, local_A_dimensions_subarray, local_A_start_coordinates, MPI_ORDER_C, MPI_INT, &local_A_subarray_type);
        MPI_Type_commit(&local_A_subarray_type);
        MPI_Sendrecv_replace(&local_A, 1 , local_A_subarray_type, neighbours_ranks[UP], 0, neighbours_ranks[DOWN], 0, grid_communicator, MPI_STATUS_IGNORE);

        // shift down matrix B (shift left)
        MPI_Type_create_subarray(2,  local_B_dimensions_full_array, local_B_dimensions_subarray, local_B_start_coordinates, MPI_ORDER_C, MPI_INT, &local_B_subarray_type);
        MPI_Type_commit(&local_B_subarray_type);
        MPI_Sendrecv_replace(&local_B, 1 , local_B_subarray_type, neighbours_ranks[LEFT], 0, neighbours_ranks[RIGHT], 0, grid_communicator, MPI_STATUS_IGNORE);
        
        pivot++;
        pivot = pivot % N_each;
    }
    if (debug){
        if(rank == check_rank){
            for (int i = 0;i < N_each;i++){
                for (int j = 0;j < N_each;j++){
                    printf("%d ",local_result[i][j]);
                }
                printf("\n");
            }
        }
    }

    //gather all the grid array into 1 result matrix
    MPI_Datatype result_subarray_type;
    int start_coordinates_result[2] = { 0,0 };
    int dimensions_full_array_result[2] = { N_each, N_each};
    int dimensions_subarray_result[2] = { N_each, N_each};
    MPI_Type_create_subarray(2,  dimensions_full_array_result, dimensions_subarray_result, start_coordinates_result, MPI_ORDER_C, MPI_INT, &result_subarray_type);
    MPI_Type_commit(&result_subarray_type);
    MPI_Send(local_result, 1, result_subarray_type, 0, 0, grid_communicator);
    MPI_Barrier(grid_communicator);

    if(debug){
        if(rank == main_rank){
            printf("gather all the result\n");
        }
    }

    //gather all result
    if(rank == main_rank){
        MPI_Datatype subarray_type;
        for (int i = 0;i < size;i++){
            int start_coordinates[2] = { ((int)(i / size_root) * N_each) , ((i % size_root) * N_each) };
            if (debug){
                printf("%d, %d \n", start_coordinates[0],start_coordinates[1]);
            }
            MPI_Type_create_subarray(2,  dimensions_full_array, dimensions_subarray, start_coordinates, MPI_ORDER_C, MPI_INT, &subarray_type);
            MPI_Type_commit(&subarray_type);

            MPI_Recv(result_all , 1 , subarray_type , i , 0 , grid_communicator , MPI_STATUS_IGNORE);
        }
    }

    if(rank == main_rank){
        printf("Result Matrix : \n");
        for(int i = 0;i < N;i++){
            for(int j = 0; j < N;j++){
                printf("%d ",result_all[i][j]);
            }
            printf("\n");
        }
    }

    //end of result 
	if (rank == 0){
		end = clock();
		double processing_time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Processing Time : %lf s",processing_time);
	}
	MPI_Finalize();
	return 0;
}


// [90, 100, 110, 120],
// [202, 228, 254, 280],
// [314, 356, 398, 440],
// [426, 484, 542, 600]