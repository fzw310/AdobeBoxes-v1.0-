#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"
#include <assert.h>
#include <vector>
#include <algorithm>
#include <omp.h>
using namespace std;

typedef struct { 
	double **coarseWin;
	int coarseWinCol;
	int coarseWinRow;
	double **spBoxes;
	int spBoxesCol;
	int spBoxesRow;
	int **segLabel;
	int segLabelCol;
	int segLabelRow;
	double **colorHist;
	int colorHistCol;
	int colorHistRow;
	double **spSize;
	int spSizeRow;
	int spSizeCol;
	double **ratioMatrix;
	int ratioMatrixRow;
	int ratioMatrixCol;
	double **distanceMatrix;
	int distanceMatrixRow;
	int distanceMatrixCol;
} option;

#define min(a,b) a<b?a:b
#define max(a,b) a>b?a:b
typedef struct { int idx; double s; } DataforSort;
typedef vector<DataforSort> Data;
bool dataCompare(const DataforSort &a, const DataforSort &b) { return a.s>b.s; }

void adobeBoxes(option &Opt, double* refineBoxes, double* score)
{
	int i = 0, j = 0, k = 0;
	int c = 0, r = 0, w = 0, h = 0;
	int size = 0;

	for (i = 0; i < Opt.coarseWinRow; i++)
	{
		vector<int>::iterator iterator;
		vector<int> backgroundLabel, candidateLabel, seedLabel, adobeLabel, internalLabel(5000);
		vector<double> histogramBG, histogramObj;

		c = Opt.coarseWin[i][0]-1; r = Opt.coarseWin[i][1]-1;
		w = Opt.coarseWin[i][2];   h = Opt.coarseWin[i][3];

		size = w * h;
		//Label of local backgournd superpixel(Sbg)
		for (j = r; j < r + h; j++) backgroundLabel.push_back(Opt.segLabel[j][c]);
		for (j = r; j < r + h; j++) backgroundLabel.push_back(Opt.segLabel[j][c + w - 1]);
		for (j = c; j < c + w; j++) backgroundLabel.push_back(Opt.segLabel[r][j]);
		for (j = c; j < c + w; j++) backgroundLabel.push_back(Opt.segLabel[r + h - 1][j]);

		sort(backgroundLabel.begin(), backgroundLabel.end());
		backgroundLabel.erase(unique(backgroundLabel.begin(), backgroundLabel.end()), backgroundLabel.end());

		//Label of candidate superpixel(Sca)
		for (j = 0; j < Opt.spBoxesRow; j++) { if (Opt.ratioMatrix[i][j] == 1) candidateLabel.push_back(j); }

		//Label of internal superpixel(Sin)
		sort(backgroundLabel.begin(), backgroundLabel.end());
		sort(candidateLabel.begin(), candidateLabel.end());
		iterator = set_difference(candidateLabel.begin(), candidateLabel.end(), backgroundLabel.begin(), backgroundLabel.end(), internalLabel.begin());
		int internalSPNum = iterator - internalLabel.begin();
		internalLabel.resize(internalSPNum);

		if (internalSPNum < 4){
			*(refineBoxes + i + 0 * Opt.coarseWinRow) = double(c+1);
			*(refineBoxes + i + 1 * Opt.coarseWinRow) = double(r+1);
			*(refineBoxes + i + 2 * Opt.coarseWinRow) = double(w);
			*(refineBoxes + i + 3 * Opt.coarseWinRow) = double(h);
			int sum = 0;
			for (j = 0; j < candidateLabel.size(); j++){ sum += Opt.spSize[candidateLabel[j]][0]; }

			*(score + i + 0 * Opt.coarseWinRow) = double(sum) / double(size);
			continue;
		}

		//distance between the background subset(Sbg) and each superpixel in the internal subset(Sin)  
		Data data(internalSPNum);
		for (j = 0; j < internalSPNum; j++)
		{
			double sum = 0;
			for (k = 0; k < backgroundLabel.size(); k++)
				sum += Opt.distanceMatrix[internalLabel[j] * Opt.spBoxesRow + backgroundLabel[k]][0];
			data[j].s = sum / backgroundLabel.size();
			data[j].idx = internalLabel[j];
		}

		//Label of adobe seed superpixel(Sse)
		sort(data.begin(), data.end(), dataCompare);
		double sum = 0;
		for (j = 0; j < data.size(); j++){
			sum += Opt.spSize[data[j].idx][0];
			seedLabel.push_back(data[j].idx);
			if (sum > 0.25 * size) break;
		}

		//distance between the background subset(Sbg) and each superpixel in the candidate subset(Sca)
		double* distance2boundary = (double *)malloc(sizeof(double)*candidateLabel.size());
		for (j = 0; j < candidateLabel.size(); j++)
		{
			double sum = 0;
			for (k = 0; k < backgroundLabel.size(); k++)
				sum += Opt.distanceMatrix[candidateLabel[j] * Opt.spBoxesRow + backgroundLabel[k]][0];
			distance2boundary[j] = sum / backgroundLabel.size();
		}

		//distance between the adobe seed subset (Sse) and each superpixel in the candidate subset(Sca)
		double* distance2forground = (double *)malloc(sizeof(double)*candidateLabel.size());
		for (j = 0; j < candidateLabel.size(); j++)
		{
			double sum = 0;
			for (k = 0; k < seedLabel.size(); k++)
				sum += Opt.distanceMatrix[candidateLabel[j] * Opt.spBoxesRow + seedLabel[k]][0];
			distance2forground[j] = sum / seedLabel.size();
		}

		//get the label for the object
		for (j = 0; j < candidateLabel.size(); j++)
			if (distance2forground[j] <= distance2boundary[j]) adobeLabel.push_back(candidateLabel[j]);

		if (adobeLabel.size() == 0)
		{
			*(refineBoxes + i + 0 * Opt.coarseWinRow) = double(c+1);
			*(refineBoxes + i + 1 * Opt.coarseWinRow) = double(r+1);
			*(refineBoxes + i + 2 * Opt.coarseWinRow) = double(w);
			*(refineBoxes + i + 3 * Opt.coarseWinRow) = double(h);
			int sum = 0;
			for (j = 0; j < internalSPNum; j++){ sum += Opt.spSize[internalLabel[j]][0]; }
			*(score + i + 0 * Opt.coarseWinRow) = double(sum) / double(size);
			continue;
		}

		//output the adobe box
		int x1 = 100000, x2 = 0, y1 = 100000, y2 = 0;
		for (j = 0; j < adobeLabel.size(); j++)
		{
			x1 = min(x1, Opt.spBoxes[adobeLabel[j]][0]);
			y1 = min(y1, Opt.spBoxes[adobeLabel[j]][1]);
			x2 = max(x2, Opt.spBoxes[adobeLabel[j]][0] + Opt.spBoxes[adobeLabel[j]][2] - 1);
			y2 = max(y2, Opt.spBoxes[adobeLabel[j]][1] + Opt.spBoxes[adobeLabel[j]][3] - 1);
		}
		w = x2 - x1 + 1;
		h = y2 - y1 + 1;

		if (w == 1 || h == 1)//give up the deformed boxes
		{
			*(refineBoxes + i + 0 * Opt.coarseWinRow) = double(c+1);
			*(refineBoxes + i + 1 * Opt.coarseWinRow) = double(r+1);
			*(refineBoxes + i + 2 * Opt.coarseWinRow) = double(w);
			*(refineBoxes + i + 3 * Opt.coarseWinRow) = double(h);
			*(score + i + 0 * Opt.coarseWinRow) = 0;
			continue;
		}

		*(refineBoxes + i + 0 * Opt.coarseWinRow) = double(x1);
		*(refineBoxes + i + 1 * Opt.coarseWinRow) = double(y1);
		*(refineBoxes + i + 2 * Opt.coarseWinRow) = double(w);
		*(refineBoxes + i + 3 * Opt.coarseWinRow) = double(h);

		sum = 0;
		for (j = 0; j < adobeLabel.size(); j++)
			sum += Opt.spSize[adobeLabel[j]][0];
		*(score + i) = double(sum) / double(w*h);
		free(distance2boundary);
		free(distance2forground);		
	}
}


void mexFunction(int nout, mxArray *out[], int nin, const mxArray *in[])
{
	int col1 = mxGetN(in[0]); int row1 = mxGetM(in[0]);

	double *rawData1 = mxGetPr(in[0]);//coarse proposals
	int col2 = mxGetN(in[1]);int row2 = mxGetM(in[1]);
	double *rawData2 = mxGetPr(in[1]);//super pixels boxes
	int col3 = mxGetN(in[2]);int row3 = mxGetM(in[2]);
	double *rawData3 = mxGetPr(in[2]);//segmentation label image
	int col4 = mxGetN(in[3]);int row4 = mxGetM(in[3]);
	double *rawData4 = mxGetPr(in[3]);//color histgram
	int col5 = mxGetN(in[4]);int row5 = mxGetM(in[4]);
	double *rawData5 = mxGetPr(in[4]);//super pixels size
	int col6 = mxGetN(in[5]);int row6 = mxGetM(in[5]);
	double *rawData6 = mxGetPr(in[5]);//ratio matix
	int col7 = mxGetN(in[6]);int row7 = mxGetM(in[6]);
	double *rawData7 = mxGetPr(in[6]);//distance matix

	int i, j;
	double **data1, **data2, **data4, **data5, **data6, **data7;
	int **data3;

	double *res = NULL;//refine boxes
	double *res1 = NULL;//objectness score

	data1 = (double **)malloc(sizeof(double*)*row1);
	for (i = 0; i < row1; i++) data1[i] = (double *)malloc(sizeof(double)*col1);
	data2 = (double **)malloc(sizeof(double*)*row2);
	for (i = 0; i < row2; i++) data2[i] = (double *)malloc(sizeof(double)*col2);
	data3 = (int **)malloc(sizeof(int*)*row3);
	for (i = 0; i < row3; i++) data3[i] = (int *)malloc(sizeof(int)*col3);
	data4 = (double **)malloc(sizeof(double*)*row4);
	for (i = 0; i < row4; i++) data4[i] = (double *)malloc(sizeof(double)*col4);
	data5 = (double **)malloc(sizeof(double*)*row5);
	for (i = 0; i < row5; i++) data5[i] = (double *)malloc(sizeof(double)*col5);
	data6 = (double **)malloc(sizeof(double*)*row6);
	for (i = 0; i < row6; i++) data6[i] = (double *)malloc(sizeof(double)*col6);
	data7 = (double **)malloc(sizeof(double*)*row7);
	for (i = 0; i < row7; i++) data7[i] = (double *)malloc(sizeof(double)*col7);

	option Opt;
	for (i = 0; i<row1; i++)for (j = 0; j<col1; j++)data1[i][j] = rawData1[i + j*row1];
	Opt.coarseWin = data1; Opt.coarseWinCol = col1; Opt.coarseWinRow = row1;
	for (i = 0; i<row2; i++)for (j = 0; j<col2; j++)data2[i][j] = rawData2[i + j*row2];
	Opt.spBoxes = data2; Opt.spBoxesCol = col2; Opt.spBoxesRow = row2;
	for (i = 0; i<row3; i++)for (j = 0; j<col3; j++)data3[i][j] = int(rawData3[i + j*row3]) - 1;
	Opt.segLabel = data3; Opt.segLabelCol = col3; Opt.segLabelRow = row3;
	for (i = 0; i<row4; i++)for (j = 0; j<col4; j++)data4[i][j] = rawData4[i + j*row4];
	Opt.colorHist = data4; Opt.colorHistCol = col4;	Opt.colorHistRow = row4;
	for (i = 0; i<row5; i++)for (j = 0; j<col5; j++)data5[i][j] = rawData5[i + j*row5];
	Opt.spSize = data5;	Opt.spSizeRow = col5; Opt.spSizeCol = row5; 
	for (i = 0; i<row6; i++)for (j = 0; j<col6; j++)data6[i][j] = rawData6[i + j*row6];
	Opt.ratioMatrix = data6; Opt.ratioMatrixRow = col6;	Opt.ratioMatrixCol = row6;
	for (i = 0; i<row7; i++)for (j = 0; j<col7; j++)data7[i][j] = rawData7[i + j*row7];
    Opt.distanceMatrix = data7;	Opt.distanceMatrixRow = row7; Opt.distanceMatrixCol = col7;

	out[0] = mxCreateDoubleMatrix(row1, col1, mxREAL);//refine boxes
	res = mxGetPr(out[0]);
	out[1] = mxCreateDoubleMatrix(1, row1, mxREAL);//score
	res1 = mxGetPr(out[1]);
	
	adobeBoxes(Opt, res, res1);

	for (i = 0; i < row1; i++)free(data1[i]);
	free(data1);
	for (i = 0; i < row2; i++)free(data2[i]);
	free(data2);
	for (i = 0; i < row3; i++)free(data3[i]);
	free(data3);
	for (i = 0; i < row4; i++)free(data4[i]);
	free(data4);
	for (i = 0; i < row5; i++)free(data5[i]);
	free(data5);
	for (i = 0; i < row6; i++)free(data6[i]);
	free(data6);
	for (i = 0; i < row7; i++)free(data7[i]);
	free(data7);
	return;
}