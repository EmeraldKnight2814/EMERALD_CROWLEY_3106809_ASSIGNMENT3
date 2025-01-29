/*
Author: Emerald Crowley
Date: Began 26/11/2022, Finished 16/12/2022
Class: Distributed Systems


Description: 
This program will multiply two matrices together using the stripe method of matrix multiplication. 
You will have three matrices A,B and C that represent the operation A x B = C. 
You will be required to determine how many nodes are in your compute group and create even stripes for A to be divided amongst the nodes. 
All nodes will receive a copy of B. 
Each node will do a matrix multiplication of its stripe of A with the matrix B to generate its stripe of C. 
Finally all nodes will use the gather operation to send back the stripes to the coordinator node wherein it will be reassembled into a single matrix.


PLEASE NOTE: This program works with some key assumptions
Assumption 1: All matricies put into this program are Square (N x N)
Assumption 2: N is evenly divisible by all nodes

matA.dat:
5, 2, 0, 4, 3, 1, 5, 9,
0, 5, 5, 9, 1, 4, 6, 7,
0, 0, 7, 1, 4, 4, 8, 0,
3, 9, 4, 7, 0, 3, 5, 3,
6, 1, 3, 7, 6, 9, 6, 6,
5, 9, 5, 2, 4, 6, 0, 7,
6, 4, 9, 9, 0, 7, 6, 1,
5, 5, 3, 2, 4, 3, 5, 6

matB.dat:
3, 6, 5, 1, 6, 3, 8, 9,
2, 5, 6, 9, 0, 1, 2, 3,
5, 8, 1, 9, 5, 9, 2, 7,
3, 8, 7, 9, 0, 9, 0, 6,
1, 8, 8, 3, 6, 8, 7, 3,
9, 4, 5, 8, 8, 3, 8, 0,
7, 0, 3, 8, 7, 3, 7, 4,
9, 2, 1, 0, 2, 8, 9, 8

multiplication result SHOULD BE:
159	118	118	116	109	167	189	176
204	175	151	254	119	225	164	187
134	112	90	180	147	140	130	99
157	169	155	247	103	165	136	168
239	217	205	243	213	250	266	210
185	201	167	209	141	193	207	187
212	230	180	308	181	231	181	215
166	151	140	171	140	169	197	173

*/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mpi.h>

#pragma warning(disable: 4996) //Blocks C4996 warning which is caused by FILE fopen function

// Print a 2D Matrix to the console.
void printMatrix(int* matrix, int matrix_size, int row_size) {
    std::cout << "[";
    for (int i = 0; i < matrix_size; i++) {
        if (i % row_size == 0 and i != 0) {
            std::cout << "]" << std::endl;
            std::cout << "[" << matrix[i];
        }
        if (i == 0) {
            std::cout << matrix[i];
        }
        else{
            std::cout << ", " << matrix[i];
        }
    }
    std::cout << "]" << std::endl;
}

// multiply a row of matrix A with a column from matrix B that will return a single value. 
// That value is the dot product of the row and column.
int dotProduct(int* matrix_a, int* matrix_b, int size) {
    int dot_product = 0;

    for (int i = 0; i < size; i++) {
        dot_product += matrix_a[i] * matrix_b[i];
    }

    return dot_product;
}

// takes in a stripe of A, a matrix of B, and computes a stripe of C.
int* multiplyStripe(int* stripe, int* matrix, int stripe_size, int matrix_size, int world_size, int row_size) {
    int* stripe_c = new int[stripe_size];

    // initialize stipe_c
    for (int i = 0; i < stripe_size; i++) {
        stripe_c[i] = 0;
    }

    int stripe_rows = row_size / world_size;
    int index = 0;

    /* My specific iteration of this loop will likely be different from other students.here is my take
     *  First, I will create an array that will store a row of A
     * Then, I will create an array that will store a collumn of B
     * Finally, i pass both arrays to DotProduct.
     * This method allows the dot product method to be less code, which is why I took it.
    */

    for (int row = 0; row < stripe_rows; row++) {
        for (int col = 0; col < row_size; col++) {
            //row of A:
            int* row_a = new int[row_size];
            for (int i = 0; i < row_size; i++) {
                // Index of the row takes a similar approach to index of C. Current index + (row * row_size)
                // The current index tells us where in the row we are
                // The (row * row_size) tells us which row we are currently on
                index = i + (row * row_size);
                row_a[i] = stripe[index];
            }

            //collumn of B:
            int* col_b = new int[row_size];
            for (int j = 0; j < row_size; j++) {
                // Index of the Collumn
                // j * row_size tells us which place on the collumn we are at
                // col tells us which collumn we are in
                index = (j * row_size) + col;
                col_b[j] = matrix[index];
            }

            // col tells us which collumn we will be writing to
            // (row_size * row) tells us which row we will be writing to
            index = col + (row_size * row);

            stripe_c[index] = dotProduct(row_a, col_b, row_size);
        }
    }
    
    return stripe_c;
}

// coordinator method (only 1 node)
void coordinator(int argc, char** argv, int world_size) {
    // size of row
    int row = atoi(argv[3]);
    // size of matrix
    int matrix = row * row;
    // size of stripe
    int stripe = matrix / world_size;

    // read matricies
    auto MATRIXA = argv[1];
    int* matrix_a = new int[matrix];

    auto MATRIXB = argv[2];
    int* matrix_b = new int[matrix];

    FILE* inputa = fopen(MATRIXA, "r");
    FILE* inputb = fopen(MATRIXB, "r");

    if (!inputa) {
        std::cout << "No file found to read called " << MATRIXA << std::endl;
        return;
    }
    if (!inputb) {
        std::cout << "No file found to read called " << MATRIXB << std::endl;
        return;
    }
    if (!(!inputa and !inputb)) {
        std::cout << "Both matrices present. Computation will be performed." << std::endl;
    }

    // read the file and store into matrix array
    fread(matrix_a, sizeof(int), matrix, inputa);
    fclose(inputa);
    fread(matrix_b, sizeof(int), matrix, inputb);
    fclose(inputb);

    // broadcast size of matrix, stripe, and row to all participants
    MPI_Bcast(&matrix, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&stripe, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // partition memory for stripes and matricies
    int* sr_partition = new int[stripe];

    // scatter stripes of A
    int* buffer_a = matrix_a;
    MPI_Scatter(buffer_a, stripe, MPI_INT, sr_partition, stripe, MPI_INT, 0, MPI_COMM_WORLD);

    // broadcast full copy of B
    MPI_Bcast(matrix_b, matrix, MPI_INT, 0, MPI_COMM_WORLD);

    // perform multiplication
    int* stripe_c = new int[stripe];

    // multiply
    stripe_c = multiplyStripe(sr_partition, matrix_b, stripe, matrix, world_size, row);

    // gather all stripes of C to coordinator
    int* matrix_c = new int[matrix];
    MPI_Gather(stripe_c, stripe, MPI_INT, matrix_c, stripe, MPI_INT, 0, MPI_COMM_WORLD);

    // print out matrices
    std::cout << "Matrix A; " << std::endl;
    printMatrix(matrix_a, matrix, row);
    std::cout << std::endl;
    std::cout << "Matrix B:" << std::endl;
    printMatrix(matrix_b, matrix, row);
    std::cout << std::endl;
    std::cout << "Matrix C:" << std::endl;
    printMatrix(matrix_c, matrix, row);
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "Daniel Crowley -- 3106809" << std::endl;

    // de-allocate memory
    delete matrix_a;
    delete matrix_b;
    delete matrix_c;
    delete sr_partition;
    delete stripe_c;
}

// participant method (all nodes that aren't the coordinator)
void participant(int world_size) {
    // broadcast size of matrix, stripe, and row to all participants
    int matrix = 0;
    MPI_Bcast(&matrix, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int row = 0;
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int stripe = 0;
    MPI_Bcast(&stripe, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // partition memory for stripes and matricies
    int* sr_partition = new int[stripe];

    // scatter stripes of A
    MPI_Scatter(NULL, stripe, MPI_INT, sr_partition, stripe, MPI_INT, 0, MPI_COMM_WORLD);

    // broadcast full copy of B
    int* matrix_b = new int[matrix];
    MPI_Bcast(matrix_b, matrix, MPI_INT, 0, MPI_COMM_WORLD);

    // perform multiplication
    int* partition = new int[stripe];

    //multiply
    partition = multiplyStripe(sr_partition, matrix_b, stripe, matrix, world_size, row);

    // gather all stripes of C to coordinator
    MPI_Gather(partition, stripe, MPI_INT, NULL, NULL, NULL, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv)
{
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        coordinator(argc, argv, world_size);
    }
    else {
        participant(world_size);
    }

    MPI_Finalize();
}
