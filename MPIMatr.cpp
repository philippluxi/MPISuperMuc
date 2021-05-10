#include "stdio.h"
#include "mpi.h"
#define AR               6        /* number of rows of a */
#define AC               2        /* number of columns of a */
#define BR               2        /* number of rows of b */
#define BC               4        /* number of columns of b */

int c[AR][BC];

int main(int argc, char **argv)
{
    int numtasks,taskid,numworkers,source,dest,rows,offset,i,j,k;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    numworkers = numtasks-1;

    /*---------------------------- master ----------------------------*/
    if (taskid == 0) {
        int a[AR][AC] = {
            {1, 2},
            {3, 4},
            {5, 6},
            {7, 8},
            {9, 10},
            {11, 12}
        };
        int b[BR][BC] = {
                {1, 2, 3, 4},
                {5, 6, 7, 8}
        };

        rows = AR/numworkers;
        offset = 0;

        for (dest=1; dest<=numworkers; dest++)
        {
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows*AC, MPI_INT,dest,1, MPI_COMM_WORLD);
            MPI_Send(&b, BR*BC, MPI_INT, dest, 1, MPI_COMM_WORLD);
            offset += rows;
        }

        for (source=1; source<=numworkers; source++)
        {
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&c[offset][0], rows*BC, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (i=0; i<AR; i++) {
            for (j=0; j<BC; j++)
                printf("%i   ", c[i][j]);
            printf ("\n");
        }
    } else {
        int a[AR][AC];
        int b[BR][BC];

        MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&a, rows*AC, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b, BR*BC, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (k=0; k<BC; k++)
            for (i=0; i<rows; i++) {
                c[i][k] = 0;
                for (j=0; j<AC; j++)
                    c[i][k] = c[i][k] + a[i][j] * b[j][k];
            }


        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c, rows*BC, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
