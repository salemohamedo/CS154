/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
 	                   int v0,v1,v2,v3,v4,v5,v6,v7;
              //      int* add_a;
		//    int* add_b; 
                  //  int v0 = 0;
		    //int v1 = 0;
	//because ints are fourbytes, and block size is thirtytwo, each line can hold eight ints, thus can have eightxeight matrices blocking 		
	//i and j loops iterate over the blocks
	//x and y perform transpose of individual blocks
     
/*        printf("made it here");
            for(int i = 0; i < M; i += 8)
                for(int j = 0; j < N; j += 8)
                    for(int x = i; x < i + 8; x++){
			for(int y = j; y < j + 8; y++){
				if(x == y){
					v0 = A[x][y];
					v1 = x;}
				else B[y][x] = A[x][y];
				}
			if(i == j) B[v1][v1] = v0;}	*/	
	      if((N==32)&&(M==32)){
	            for(int i = 0; i < M; i += 8){
	                for(int j = 0; j < N; j += 8){
	 	              //for(int j = 0; j < N; j += 8){}
		                    for(int x = 0; x < 8; x++){            	
                    v0 = A[j + x ][i + 0];
                    v1 = A[j + x ][i + 1];
                    v2 = A[j + x ][i + 2];
                    v3 = A[j + x ][i + 3];
                    v4 = A[j + x ][i + 4];
                    v5 = A[j + x ][i + 5];
                    v6 = A[j + x ][i + 6];	
                    v7 = A[j + x ][i + 7];
                    B[i + 0][j + x ] = v0;
                    B[i + 1][j + x ] = v1;
                    B[i + 2][j + x ] = v2;
                    B[i + 3][j + x ] = v3;
                    B[i + 4][j + x ] = v4;
                    B[i + 5][j + x ] = v5;
                    B[i + 6][j + x ] = v6;
                    B[i + 7][j + x ] = v7;
                }}}}
                    						                    			        
	//same as 32 32 except different block size
	if((N==64)&&(M==64)){
		for(int i = 0; i < 64; i += 4)
			for(int j = 0; j < 64; j += 4)
				for(int x = j; x < j + 4; x++){
					for(int y = i; y < i + 4; y++){
						if(x == y){
							v0 = A[x][y];
							v1 = x;}
						else B[y][x] = A[x][y];}
					if(i == j) B[v1][v1] = v0;}}
	//same idea as 32 32 without dealing w diagonals and addition of boundaries for x and y to not exceed 32 and 64 respectively given rectangular matrix
	if((N==64)&&(M==32))
		for(int i = 0; i < 64; i += 8)
			for(int j = 0; j < 32; j += 8)
				for(int x = j; ((x < j + 8)&&(x < 32)); x++)
					for(int y = i; ((y < i + 8)&&(y < 64)); y++){
						v0 = A[y][x];
						B[x][y] = v0;}
	//again using 8x8 blocks
		if(N==67)
			for(int i = 0; i < 67; i += 8)
				for(int j = 0; j < 61; j += 8)
					for(int x = 0; (x < 8)&&(x + j < 61); x++)
							for(int y = 0; (y < 8)&&(y + i < 67); y++)
								B[x + j][y + i] = A[y + i][x + j];
									


}

				

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}



