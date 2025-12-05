#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){

	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);

    // const int N = 5;
    // int matrix_A[N][N];
    // int matrix_B[N][N];

    // int check_rank = 0;
    // if(rank == 0){
    //     int count = 0;
    //     for (int i = 0; i < N;i++){
    //         for (int j = 0; j < N;j++){
    //                 matrix_A[i][j] = count;
    //                 matrix_B[i][j] = count;
    //         }
    //     }
    // }

    enum role_ranks { SENDER, RECEIVER };
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    switch(my_rank)
    {
        case SENDER:
        {
            // Declare the full array
            int full_array[4][4];
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    full_array[i][j] = i * 4 + j;
                }
            }
 
            // Create the subarray datatype
            MPI_Datatype subarray_type;
            int dimensions_full_array[2] = { 4, 4 };
            int dimensions_subarray[2] = { 2, 3 }; //size of subarray
            int start_coordinates[2] = { 2, 1 };
            MPI_Type_create_subarray(2,  dimensions_full_array, dimensions_subarray, start_coordinates, MPI_ORDER_C, MPI_INT, &subarray_type);
            // dim_count, array_element_counts, subarray_element_counts, subarray_coordinates
            MPI_Type_commit(&subarray_type); //MPI_Type_commit
 
            // Send the message
            printf("MPI process %d sends:\n-  -  -  -\n-  -  -  -\n-  %d %d %d\n- %d %d %d\n", my_rank, full_array[2][1], full_array[2][2], full_array[2][3], full_array[3][1], full_array[3][2], full_array[3][3]);
            MPI_Send(full_array, 1, subarray_type, RECEIVER, 0, MPI_COMM_WORLD);
            break;
        }
        case RECEIVER:
        {
            int main_matrix[4][4];
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    main_matrix[i][j] = 0;
                }
            }

                
            int ndims = 2;
            int array_sizes[2] = {4, 4};
            int sub_sizes[2] = { 2, 3 }; //size of subarray
            int starts[2] = { 2, 1 };
            MPI_Datatype recv_subarray_type;

            // Receive the message
            // int received[6];
            // MPI_Recv(&received, 6, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("start receive\n");

            MPI_Type_create_subarray(ndims, array_sizes, sub_sizes, starts, MPI_ORDER_C, MPI_INT, &recv_subarray_type);
            MPI_Type_commit(&recv_subarray_type);
            MPI_Recv(main_matrix, 1, recv_subarray_type, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            printf("done receive\n");

            // printf("MPI process %d receives:\n%d %d %d %d %d %d\n", my_rank, received[0], received[1], received[2], received[3], received[4], received[5]);

            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    printf("%d ",main_matrix[i][j]);
                }
                printf("\n");
            }
            break;
        }
    }
    
	
	MPI_Finalize();
	return 0;
	

}