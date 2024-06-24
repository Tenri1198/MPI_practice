//  mpic++ circuit1.cpp
//  mpirun -hostfile ~/hostfile -np 4 ./a.out
/*
 *   Circuit Satisfiability, Version 1
 *
 *   This MPI program determines whether a circuit is
 *   satisfiable, that is, whether there is a combination of
 *   inputs that causes the output of the circuit to be 1.
 *   The particular circuit being tested is "wired" into the
 *   logic of function 'check_circuit'. All combinations of
 *   inputs that satisfy the circuit are printed.
 *
 *   Programmed by Michael J. Quinn
 *
 *   Last modification: 3 September 2002
 */

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <iostream>


/* Return 1 if 'i'th bit of 'n' is 1; 0 otherwise */
#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)

void check_circuit (int id, int z, int* local_solutions) {
   int v[16];        /* Each element is a bit of z */
   int i;

   for (i = 0; i < 16; i++) v[i] = EXTRACT_BIT(z,i);
   if ((v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
      && (!v[3] || !v[4]) && (v[4] || !v[5])
      && (v[5] || !v[6]) && (v[5] || v[6])
      && (v[6] || !v[15]) && (v[7] || !v[8])
      && (!v[7] || !v[13]) && (v[8] || v[9])
      && (v[8] || !v[9]) && (!v[9] || !v[10])
      && (v[9] || v[11]) && (v[10] || v[11])
      && (v[12] || v[13]) && (v[13] || !v[14])
      && (v[14] || v[15])) {
      printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
         v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],
         v[10],v[11],v[12],v[13],v[14],v[15]);
      fflush (stdout);
      *local_solutions = *local_solutions+1;
   }
}

int main(int argc, char *argv[])
{
    int i;
    int id;     //processor rank    
    int p;      //Number of processes
    double t_start = 0.;
    double t_end = 0.;
    double elapsed_time = 0.;
    double max_time = 0.;
    int global_solutions=0;
    int local_solutions=0;
    
    FILE *fp = NULL;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

    for(i=id; i<65536; i+=p)
    {
        check_circuit(id, i, &local_solutions);
    }

    MPI_Reduce(&local_solutions, &global_solutions, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    t_end = MPI_Wtime();
    elapsed_time = t_end - t_start;
    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if(id==0)
    {
        printf("Max Time: %lf\n", max_time);
        fflush(stdout);
        printf("There are %d different solutions\n", global_solutions);
        fflush(stdout);       
        fp = fopen("walltime.dat", "a"); // Open for appending
        if (fp != NULL) {
            fprintf(fp, "%d %lf\n", p, max_time); // Write number of processes and elapsed time
            fclose(fp);
        } else {
            fprintf(stderr, "Failed to open file for writing.\n");
        }
    }
    MPI_Finalize();
    return 0;
}