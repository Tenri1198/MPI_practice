#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

//!      == number of dimension of discrete 2D mesh
#define  M          100

//!== number of iterations for Gauss-Seidel Method
#define  MAX_ITER   10000

#define  MAX_HEAT   100.0

#define  DEBUG      1

#define  EPS        1.0e-4

/* Please define the matrices in here */
static double  U[M+2][M+2];
static double  U_old[M+2][M+2];


void MyPoisson(double U_rhs[M+2][M+2], double h_pow); 
void CalcErr(double U_rhs[M+2][M+2], double U_lhs[M+2][M+2], double *dmax);

int main(int argc, char* argv[]) {

     double  t0, t1, t2, t_w;
     double  h, h_pow, dmax;

     int     i, j;      
     int     ii, jj;


     for (i=0; i<M+2; i++) {
       for (j=0; j<M+2; j++) {
         U[i][j] = 0.0;
         U_old[i][j] = 0.0;
       }
     }

     //=== Set given temperature ----------------------------------
     for (j=0; j<M+2; j++) {
       U[0][j] = MAX_HEAT;
     }
     for (i=1; i<M+1; i++) {
       U[i][0] = MAX_HEAT - (double)i/(double)M * MAX_HEAT;
       U[i][M+1] = (double)i/(double)M * MAX_HEAT;
     }
     for (j=0; j<M+2; j++) {
       U[M+1][j] = 0.0;
     }
     //=== End of setting given temperature ------------------------


     //=== Start of Solving Poisson equation ----------------------------
     t1 = omp_get_wtime();

     h = 1.0 / (double)(M+1);
     h_pow = h*h;

     //=== Main loop
     for (i=1; i<=MAX_ITER; i++) {


       // --- perform explicit method ( Gauss-Seidel Method )
       MyPoisson(U, h_pow);
       //--- compute maximum differences
       CalcErr(U, U_old, &dmax);

       if (i%100 == 0) {
         printf("iter= %d dmax= %e  \n", i, dmax);
       }

       if (dmax < EPS) {
         printf(" %d Iteration is converged in residual %e \n", i, EPS);
         goto outloop;
       }

       //--- copy back to U_old
       for (ii=1; ii<M+1; ii++) {
         for (jj=1; jj<M+1; jj++) {
	   U_old[ii][jj] = U[ii][jj];
         }
       }

     } //=== End of Main loop

     printf("Iteration is not converged within %d times. \n",MAX_ITER);


 outloop: 

     t2 = omp_get_wtime();
     t_w =  t2 - t1; 
     //=== End of Solving Poisson equation ----------------------------

       printf("M  = %d \n",M);
       printf("MAX_ITER  = %d \n",MAX_ITER);

       printf("time = %lf [sec.] \n",t_w);


       //=== output results
       /*
       for (ii=1; ii<M+1; ii++) {
         for (jj=1; jj<M+1; jj++) {
	   printf("%d %d %lf \n",ii,jj, U[ii][jj]);
         }
         printf("\n");
       }
       */


     return 0;
}


void MyPoisson(double U_rhs[M+2][M+2], double h_pow)
{

  int     i, j;

  //=== u_{i,j} = 1/4 (h^2 f_{i,j} + u_{i,j-1} +u_{i-1,j}+u_{i+1,j}+u_{i,j+1})
  for (i=1; i<M+1; i++) {
    for (j=1; j<M+1; j++) {
      U_rhs[i][j] = 0.25 * 
        ( h_pow * sin((double)i * (double)j) + 
	  U_rhs[i][j-1] + U_rhs[i-1][j] + U_rhs[i+1][j] + U_rhs[i][j+1] );
      }
    }

}

void CalcErr(double U_rhs[M+2][M+2], double U_lhs[M+2][M+2], double *dmax)
{ 

   int  i,j;
   double dtemp;

   *dmax = 0.0;
   for (i=1; i<M+1; i++) {
     for (j=1; j<M+1; j++) {
       dtemp = fabs(U_rhs[i][j] - U_lhs[i][j]);
       if (dtemp > *dmax ) *dmax = dtemp;
     }
   }

}