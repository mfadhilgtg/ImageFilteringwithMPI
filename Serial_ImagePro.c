/* Extra Credit Homework 1 Solution (#40)                                     */
/* Yudi Gondokaryono                                                          */
/* Section 1&2                                                                */
/* November 11, 2002                                                          */
/******************************************************************************/
/* Purpose : Imgage Filtering Program                                         */
/******************************************************************************/
/* Variable Definitions (Main Function):                                      */
/* Variable Name          Type     Description                                */
/* pict[256][256]         int      image array                                */
/* new_pict[256][256]     int      new image array                            */
/* r                      int      # of row in the image                      */
/* c                      int      # of column in the image                   */
/* flt[3][3]              double   filter array                               */
/******************************************************************************/
/* Source Code:                                                               */

/* Include all library we need                                                */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/* Declare all constant                                                       */
#define FILTER_SIZE 3 // filter size

/* Begin the Main Function                                                    */
int main( void )
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

clock_t start = clock();
/* Image Filter                                                                */
/*  copy edges                                                                */
	for(i = 0; i < height; i++)
	{
		new_pict[i*height+0] = pict[i*height+0];

		new_pict[i*height+(width-1)] = pict[i*height+width-1];
	}

	for(j = 0; j < width; j++)
	{
		new_pict[j] = pict[j];
		new_pict[(height-1)*height+j] = pict[(height-1)*height+j];
	}
/*  compute coefficient                                                       */
	for(i = 0; i < FILTER_SIZE; i++)
		for(j = 0; j < FILTER_SIZE; j++)
			coeff += filter[i][j];

/*  filter the image                                                          */
	for(i = 1; i < height-1; i++)
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
		for(i = 1; i < height-1; i++)
			for(j = 1; j < width-1; j++)
				new_pict[i*height+j] = (int)(new_pict[i*height+j]/coeff);
	}

/*  check for pixel > 255 and pixel < 0                                       */
	for(i = 1; i < height-1; i++)
		for(j = 1; j < width-1; j++)
		{
			if(new_pict[i*height+j] < 0)
				new_pict[i*height+j] = 0;
			else if(new_pict[i*height+j] > 255)
				new_pict[i*height+j] = 255;
		}
	clock_t end = clock();
/* Write Image File                                                            */
	//write_pict(new_pict, height, width);
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

float seconds = (float)(end - start) / CLOCKS_PER_SEC;
printf("\nCalculation Time = \n %f seconds\n",seconds);
/* Finish                                                                     */
    free(pict);
    free(new_pict);
	return(0);
/* End Main Function                                                          */
}
