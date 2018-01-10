/* Tugas 2 EL4127 Arsitektur dan Komputasi Paralel                            */
/* Muhammad Fadhil Ginting                                                    */
/* 13213072                                                                   */
/* October 3, 2016                                                            */
/******************************************************************************/
/* Purpose : Parallel Image Filtering Program                                 */
/******************************************************************************/
/* Source Code:                                                               */           
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

/* Declare all constant                                                       */
#define FILTER_SIZE 3 // filter size
/* Paralel Declaration														  */

/* Begin the Main Function                                                    */
int main( int argc, char *argv[] )
{
	int width, height; // actual image size
	//int pict[IM_SIZE][IM_SIZE];
	int* pict;
	int* new_pict;
	double filter[FILTER_SIZE][FILTER_SIZE]={{-1.25, 0, -1.25},
										  {0, 10, 0},
									   	  {-1.25, 0, -1.25}};
    //Read/Write File variable
	int i, j, max;
	FILE *in,*out;
	char line[200];

	//Image Filter Variable
    double coeff = 0;
	double sum;
	int m, n;
	
	/*Paralel proccess variable												  */
	int	mpiRank = 0;
	int	mpiSizie = 1;
	double t;
	int sizeSent, sizeToBeSent;
	int i, j, k, rc;       /* misc */
	MPI_Status status;
	int h_local; 
	
	
//Paralel Start
MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);


/* *************************** MAIN TASK1 *********************************** */  	
if (!mpiRank)
{

/* Read Image File                                                            */
	in=fopen("original.pgm", "r");	// f16 image
	if(in == NULL)
	{
		printf("Error reading original.pgm\n");
		exit(1);
	}

	fgets(line, 199, in); // get PGM Type

	line[strlen(line)-1]='\0';  // get rid of '\n'

	if(strcmp(line, "P2") != 0)
	{
		printf("Cannot process %s PGM format, only P2 type\n", line);
		exit(2);
	}

	fgets(line, 199, in); // get comment

	fscanf(in, "%d %d", &width, &height); // get size width x height

	fscanf(in, "%d", &max); // get maximum pixel value

    pict = (int*)malloc(sizeof(int)* height * width);
    new_pict = (int*)malloc(sizeof(int)* height * width);

	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			{fscanf(in, "%d", &pict[height*i+j]);
			}
	fclose(in);
/* Read Done	                                                              */
/* Image Filter                                                               */
	t = MPI_Wtime();
	/*  compute coefficient                                                       */
	for(i = 0; i < FILTER_SIZE; i++)
		for(j = 0; j < FILTER_SIZE; j++)
			coeff += filter[i][j];
}
/* End Main Task 1                                                            */

/* Paralel Task																	  */
//Broadcast Image Size and Filter Coeff

MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Bcast(&coeff, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

//Calculate row for each task
h_local = (height / mpiSize);
if(!mpiRank)
{
	h_local = (height / mpiSize) + (height%mpiSize);
	h_local += 1;
}
else
{
	h_local = (height / mpiSize);
	h_local += 2;
	
}
if(mpiRank == mpiSize) h_local-=1;

//Make matrix in other task
if (mpiRank){
	pict = (int*)malloc(sizeof(int)* width * h_local);
	new_pict = (int*)malloc(sizeof(int)* width * h_local);
}

//Send Pict by splitting it in row-wise parts
    if (!mpiRank) {
        sizeSent = width*h_local;
        for (i=1; i<mpiSize-1; i++) {
            sizeToBeSent = width * (h_local+1);
            MPI_Send(pict + sizeSent, sizeToBeSent, MPI_INT, i, TAG_INIT,
                     MPI_COMM_WORLD);
            sizeSent += sizeToBeSent;
        }
		//Last Part sent
		sizeToBeSent = width * (h_local);
		MPI_Send(pict + sizeSent, sizeToBeSent, MPI_INT, i, TAG_INIT,
                     MPI_COMM_WORLD);
    }
    else { /* Receive parts of pict */
        MPI_Recv(pict, h_local*width, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
//Each task Filter the image

/*  copy edges                                                                */
	for(i = 0; i < h_local; i++)
	{
		new_pict[i*height+0] = pict[i*height+0];

		new_pict[i*height+(width-1)] = pict[i*height+width-1];
	}
	if(mpiRank == 0)
	{
	for(j = 0; j < width; j++)
	{
		new_pict[j] = pict[j];
	}
	}

	if(mpiRank == mpiSize)
	{
	for(j = 0; j < width; j++)
	{
		new_pict[(h_local-1)*height+j] = pict[(h_local-1)*height+j];
	}
	}
	
/*  filter the image                                                          */
	for(i = 1; i < h_local-1; i++)
		for(j = 1; j < width-1; j++)
		{
			sum = 0;
			for(m = 0; m < FILTER_SIZE; m++)
				for(n = 0; n < FILTER_SIZE; n++)
					sum += pict[((i+(m-1))*height)+(j+(n-1))]*filter[m][n];
			new_pict[i*height+j] = (int)sum;
		}

	if(coeff != 0)
	{
		for(i = 1; i < h_local-1; i++)
			for(j = 1; j < width-1; j++)
				new_pict[i*height+j] = (int)(new_pict[i*height+j]/coeff);
	}
/*  check for pixel > 255 and pixel < 0                                       */
	for(i = 1; i < h_local-1; i++)
		for(j = 1; j < width-1; j++)
		{
			if(new_pict[i*height+j] < 0)
				new_pict[i*height+j] = 0;
			else if(new_pict[i*height+j] > 255)
				new_pict[i*height+j] = 255;
		}
/* Receive partial results from each slave */

    if (!mpiRank) {
        sizeSent = width*h_local;
		h_local = (height / mpiSize);
        for (i=1; i<mpiSize; i++) {
            sizeToBeSent = width*h_local;
            MPI_Recv(new_pict + sizeSent, sizeToBeSent, MPI_INT, i, TAG_RESULT,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sizeSent += sizeToBeSent;
        }
    }
    else { /* Send partial results to master */
        h_local = (height / mpiSize);
		sizeToBeSent = h_local*width;
		MPI_Send(new_pict + width, sizeToBeSent, MPI_INT, 0, TAG_RESULT, MPI_COMM_WORLD);
    }

t = MPI_Wtime() - t;
/* *************************** MAIN TASK2 *********************************** */
/* Write Image File                                                           */
	out=fopen("new.pgm", "w");

	fprintf(out, "P2\n");
	fprintf(out, "# new.pgm\n");
	fprintf(out, "%d %d\n", width, height);
	fprintf(out, "255\n");

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
			fprintf(out, "%5d", new_pict[i*height+j]);
		fprintf(out, "\n");
	}

	fclose(out);
	
/* Finish                                                                     */
	printf("Calculation time :\n%f Seconds\n",t);
	free(pict);
    free(new_pict);	
	MPI_Finalize();
/* End Main Task 2                                                            */

/* Finish Done                                                                */
MPI_Finalize();
return(0);
/* End Main Function                                                          */  
}

