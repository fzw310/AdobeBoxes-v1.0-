#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"
#include <assert.h>

#define min(a,b) a<b?a:b
#define max(a,b) a>b?a:b

void adjustBoxes(double **data1, int row1, int col1, double **data2, int row2, int col2, double **data3, int row3, int col3, int rowImage, int colImage, double *res)
{//coarseBoxes, superPixelBoxes, ratio
	int i = 0, j = 0, m = 0, n = 0;
	double x1 = 100000, x2 = 0, y1 = 100000, y2 = 0;
	double w = 0, h = 0;
	int flag = 0;

	for (i = 0; i < row1; i++)
	{
		x1 = 100000, x2 = 0, y1 = 100000, y2 = 0;
		flag = 0;
		for (j = 0; j < row2; j++)
		{
			if (data3[i][j] == 1)
			{
				flag = 1;
				x1 = min(x1, data2[j][0]);
				y1 = min(y1, data2[j][1]);
				x2 = max(x2, data2[j][0] + data2[j][2] - 1);
				y2 = max(y2, data2[j][1] + data2[j][3] - 1);
			}
		}
		if (flag == 1)
		{
			x1 = max(x1 - 1, 1);
			y1 = max(y1 - 1, 1);
			x2 = min(x2 + 1, colImage);
			y2 = min(y2 + 1, rowImage);


			w = x2 - x1 + 1;
			h = y2 - y1 + 1;
			*(res + i + 0 * row1) = x1;
			*(res + i + 1 * row1) = y1;
			*(res + i + 2 * row1) = w;
			*(res + i + 3 * row1) = h;
		}
		else
		{
			*(res + i + 0 * row1) = data1[i][0];
			*(res + i + 1 * row1) = data1[i][1];
			*(res + i + 2 * row1) = data1[i][2];
			*(res + i + 3 * row1) = data1[i][3];
		}

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

	int col3 = mxGetN(in[2]);
	int row3 = mxGetM(in[2]);

	double *rawData3 = mxGetPr(in[2]);//coarse proposals

	double rowImage = *(double*)mxGetPr(in[3]);
	double colImage = *(double*)mxGetPr(in[4]);

	int i, j;
	double **data1;
	double **data2;
	double **data3;

	double *res = NULL;//Êä³ö½á¹û

	data1 = (double **)malloc(sizeof(double*)*row1);
	for (i = 0; i < row1; i++)
		data1[i] = (double *)malloc(sizeof(double)*col1);

	data2 = (double **)malloc(sizeof(double*)*row2);
	for (i = 0; i < row2; i++)
		data2[i] = (double *)malloc(sizeof(double)*col2);

	data3 = (double **)malloc(sizeof(double*)*row3);
	for (i = 0; i < row3; i++)
		data3[i] = (double *)malloc(sizeof(double)*col3);

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

	for (i = 0; i<row3; i++)
	{
		for (j = 0; j<col3; j++)
		{
			data3[i][j] = rawData3[i + j*row3];
		}
	}

	out[0] = mxCreateDoubleMatrix(row1, col1, mxREAL);
	res = mxGetPr(out[0]);

	adjustBoxes(data1, row1, col1, data2, row2, col2, data3, row3, col3, int(rowImage), int(colImage), res);//coarseBoxes, superPixelBoxes, ratio

	for (i = 0; i < row1; i++)
		free(data1[i]);
	free(data1);
	for (i = 0; i < row2; i++)
		free(data2[i]);
	free(data2);
	for (i = 0; i < row3; i++)
		free(data3[i]);
	free(data3);
	return;
}

