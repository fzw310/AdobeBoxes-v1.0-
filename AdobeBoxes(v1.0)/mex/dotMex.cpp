#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"



void dot(double *I1, int r1, int c1, double *I2, int r2, int c2, double *res)
{
	unsigned int i = 0, j = 0;
	unsigned int m = 0, n = 0;
	double sum = 0;
	for (i = 0; i < r1 - r2 + 1; i++)
	for (j = 0; j <c1 - c2 + 1; j++)
	{
		sum = 0;
		for (m = 0; m < r2; m++)
		for (n = 0; n < c2; n++)
		{
			sum += I1[(i + m)*c1 + j + n] * I2[m*c2 + n];
		}

		res[i + j*(r1 - r2 + 1)] = sum;
	}


}

void mexFunction(int nout, mxArray *out[], int nin, const mxArray *in[])
{
	int col1 = mxGetN(in[0]);
	int row1 = mxGetM(in[0]);
	int col2 = mxGetN(in[1]);
	int row2 = mxGetM(in[1]);

	double *I1Src = mxGetPr(in[0]);
	double *I2Src = mxGetPr(in[1]);

	int i, j;

	
	double *I1 = (double*)calloc(row1*col1, sizeof(double));
	double *I2 = (double*)calloc(row2*col2, sizeof(double));

	double *res = NULL;

	FILE *f;

	for (i = 0; i<row1; i++)for (j = 0; j<col1; j++)I1[i*col1 + j] = I1Src[i + j*row1];
	for (i = 0; i<row2; i++)for (j = 0; j<col2; j++)I2[i*col2 + j] = I2Src[i + j*row2];

	out[0] = mxCreateDoubleMatrix((row1 - row2 + 1)*(col1 - col2 + 1), 1, mxREAL);
	res = mxGetPr(out[0]);
	dot(I1, row1, col1, I2, row2, col2, res);

	free(I1);
	I1 = NULL;
	free(I2);
	I2 = NULL;
	return;
}