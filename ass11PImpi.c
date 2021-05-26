/*
 * Compute pi by approximating the area under the curve f(x) = 4 / (1 + x*x)
 * between 0 and 1.
 *
 * Must install openmpi-bin and libopenmpi-dev
 *
 * On Ubuntu, this would be:
 *	sudo apt-get install openmpi-bin
 *	sudo apt-get install libopenmpi-dev
 *
 * compile:  mpicc PImpi.c -o PImpi
 */

// --------------------------------------------------------

#include "mpi.h"
#include <stdio.h>
#include <math.h>


// --------------------------------------------------------
//  main program

int main(int argc, char *argv[])
{
	int		n, myid, numprocs, i;
	const double	PI25DT = 3.141592653589793238462643;
	double		mypi, pi, h, sum, x;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == 0)
		n = 10000000;

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	h = 1.0 / (double) n;
	sum = 0.0;

	for (i = myid + 1; i <= n; i += numprocs) {
		x = h * ((double)i - 0.5);
		sum += (4.0 / (1.0 + x*x));
	}

	mypi = h * sum;
	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (myid == 0) {
		printf("MPI program results:\n");
		printf("pi is approximately %.16f, Error is %.16f\n",
					pi, fabs(pi - PI25DT));
	}

	MPI_Finalize();
	return 0;
}

