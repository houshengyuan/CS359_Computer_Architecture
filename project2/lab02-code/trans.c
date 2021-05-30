/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * Name: Shengyuan Hou  Student ID:518021910604
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
    int i,j,k,s,a0,a1,a2,a3,a4,a5,a6,a7;
    if(M==32)                        //32*32 use the 8*8 block matrix 
    {
     for(i=0;i<32;i+=8)
      for(j=0;j<32;j+=8)
      { for(k=0;k<8;k++)            //copy of block matrix from A to B in the transpose position
        {
         a0=A[i+k][j];a1=A[i+k][j+1];a2=A[i+k][j+2];a3=A[i+k][j+3];
         a4=A[i+k][j+4];a5=A[i+k][j+5];a6=A[i+k][j+6];a7=A[i+k][j+7]; 
         B[j+k][i]=a0;B[j+k][i+1]=a1;B[j+k][i+2]=a2;B[j+k][i+3]=a3;
         B[j+k][i+4]=a4;B[j+k][i+5]=a5;B[j+k][i+6]=a6;B[j+k][i+7]=a7; 
        }
        for(k=0;k<8;k++)           //transpose of block matrix in B
          for(s=k+1;s<8;s++)
          {
           a0=B[j+k][i+s];
           B[j+k][i+s]=B[j+s][i+k];
           B[j+s][i+k]=a0;   
          }
      }
    }
    else if(M==64)                 //64*64 use the 8*8 block matrix with some tricks,see the details in the report
    {
      for(i=0;i<64;i+=8)
        for(j=0;j<64;j+=8)
       {
          for(k=0;k<4;k++)         //transpose the first four rows in A, corresponding to step (1) in the report
          {
            a0=A[i+k][j];a1=A[i+k][j+1];a2=A[i+k][j+2];a3=A[i+k][j+3];
            a4=A[i+k][j+4];a5=A[i+k][j+5];a6=A[i+k][j+6];a7=A[i+k][j+7];
            B[j+k][i]=a0;B[j+k][i+1]=a1;B[j+k][i+2]=a2;B[j+k][i+3]=a3;
            B[j+k][i+4]=a4;B[j+k][i+5]=a5;B[j+k][i+6]=a6;B[j+k][i+7]=a7;
          }
          for(k=0;k<4;k++)        //corresponding to step (1) in the report
            for(s=k+1;s<4;s++)
            {
             a0=B[j+k][i+s];
             B[j+k][i+s]=B[j+s][i+k];
             B[j+s][i+k]=a0;
             a0=B[j+k][i+s+4];
             B[j+k][i+s+4]=B[j+s][i+k+4];
             B[j+s][i+k+4]=a0; 
            }
        for(k=0;k<4;k++)         //corresponding to step (2) (3) (4) in the report
         {
           a0=A[i+4][j+k];a1=A[i+5][j+k];a2=A[i+6][j+k];a3=A[i+7][j+k];
           a4=A[i+4][j+k+4];a5=A[i+5][j+k+4];a6=A[i+6][j+k+4];a7=A[i+7][j+k+4];
           s=a0;a0=B[j+k][i+4];B[j+k][i+4]=s;
           s=a1;a1=B[j+k][i+5];B[j+k][i+5]=s;
           s=a2;a2=B[j+k][i+6];B[j+k][i+6]=s;
           s=a3;a3=B[j+k][i+7];B[j+k][i+7]=s;
           B[j+4+k][i+0]=a0; B[j+4+k][i+1]=a1;
           B[j+4+k][i+2]=a2; B[j+4+k][i+3]=a3;
           B[j+4+k][i+4]=a4; B[j+4+k][i+5]=a5;
           B[j+4+k][i+6]=a6; B[j+4+k][i+7]=a7;   
         }
       }       
    }
    else if(M==61)                   //61*67 use 18*14 block matrix
    {
       for(i=0;i<61;i+=18)
         for(j=0;j<67;j+=14)
          for(s=0;s<18&&(i+s)<61;s++)
            for(k=0;k<14&&(j+k)<67;k++)
          {
             B[i+s][j+k]=A[j+k][i+s];
          }
    }
        
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

