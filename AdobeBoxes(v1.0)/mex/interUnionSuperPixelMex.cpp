#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"
#include <assert.h>

#define min(a,b) a<b?a:b
#define max(a,b) a>b?a:b

double ratio(double *vec1, double *vec2)
{
	//vec1 - super pixels, vec2 - coarse proposals
	//x,y,w,h

	double r = 0;
	double maxX1 = max(*(vec1 + 0), *(vec2 + 0));
	double maxY1 = max(*(vec1 + 1), *(vec2 + 1));
	double minX2 = min(*(vec1 + 0) + *(vec1 + 2)-1, *(vec2 + 0) + *(vec2 + 2)-1);
	double minY2 = min(*(vec1 + 1) + *(vec1 + 3)-1, *(vec2 + 1) + *(vec2 + 3)-1);

	double w = max(0, (minX2 - maxX1 + 1));
	double h = max(0, (minY2 - maxY1 + 1));

	r = w * h / ((*(vec1 + 2)) * (*(vec1 + 3)));
	if (r > 0.9)
		r = 1;
	else
		r = 0;
		
	return r;
}

void interUnion(double **data1, int row1, int col1, double **data2, int row2, int col2, double *res)
{
	int i = 0, j = 0;
	//col1 = col2;
	int col = col1;

	for (i = 0; i < row2; i++)
	for (j = 0; j < row1; j++)
	{
		*(res + i + j * row2) = ratio(data1[j], data2[i]);
	}
}


void mexFunction(int nout, mxArray *out[], int nin, const mxArray *in[])
{
	int col1 = mxGetN(in[0]);
	int row1 = mxGetM(in[0]);

	double *rawData1 = mxGetPr(in[0]);//super pixels

	int col2 = mxGetN(in[1]);
	int row2 = mxGetM(in[1]);

	double *rawData2 = mxGetPr(in[1]);//coarse proposals

	int i, j;
	double **data1;
	double **data2;

	double *res = NULL;//output

	data1 = (double **)malloc(sizeof(double*)*row1);
	for (i = 0; i < row1; i++)
		data1[i] = (double *)malloc(sizeof(double)*col1);

	data2 = (double **)malloc(sizeof(double*)*row2);
	for (i = 0; i < row2; i++)
		data2[i] = (double *)malloc(sizeof(double)*col2);

	for (i = 0; i<row1; i++)
	{
		for (j = 0; j<col1; j++)
		{
			data1[i][j] = rawData1[i + j*row1];
		}
	}

	for (i = 0; i<row2; i++)
	{
		for (j = 0; j<col2; j++)
		{
			data2[i][j] = rawData2[i + j*row2];
		}
	}

	out[0] = mxCreateDoubleMatrix(row2 , row1, mxREAL);
	res = mxGetPr(out[0]);

	interUnion(data1, row1, col1, data2, row2, col2, res);

	for (i = 0; i < row1; i++)
		free(data1[i]);
	free(data1);
	for (i = 0; i < row2; i++)
		free(data2[i]);
	free(data2);
	return;
}

