#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"
#include <assert.h>
#include <vector>
#include <algorithm>

using namespace std;
typedef struct { int c, r, w, h; float s; } Box;
typedef vector<Box> Boxes;
bool boxesCompare(const Box &a, const Box &b) { return a.s>b.s; }
int clamp(int v, int a, int b) { return v<a ? a : v>b ? b : v; }

float boxesOverlap(Box &a, Box &b) {
	float areai, areaj, areaij;
	int r0, r1, c0, c1, r1i, c1i, r1j, c1j;
	r1i = a.r + a.h; c1i = a.c + a.w; if (a.r >= r1i || a.c >= c1i) return 0;
	r1j = b.r + b.h; c1j = b.c + b.w; if (a.r >= r1j || a.c >= c1j) return 0;
	areai = (float)a.w*a.h; r0 = max(a.r, b.r); r1 = min(r1i, r1j);
	areaj = (float)b.w*b.h; c0 = max(a.c, b.c); c1 = min(c1i, c1j);
	areaij = (float)max(0, r1 - r0)*max(0, c1 - c0);
	return areaij / (areai + areaj - areaij);
}

void boxesNms(Boxes &boxes, float thr, Boxes &NMSBoxes)
{
	Box firstBox;

	sort(boxes.begin(), boxes.end(), boxesCompare);
	if (thr>.99) return;
	while (boxes.size() != 0)
	{
		NMSBoxes.push_back(boxes[0]);
		firstBox = boxes[0];
		float t = 0;
		if (firstBox.w * firstBox.h > 64 * 64) t = thr;
		else t = thr + 0.1;

		Boxes::iterator it;
		for (it = boxes.begin(); it < boxes.end(); )
		{ 
			float r = boxesOverlap(firstBox, *it);
			if (r >= t)	it = boxes.erase(it);
			else it++;				
		}
	}
	int a = 0;
}

void mexFunction(int nout, mxArray *out[], int nin, const mxArray *in[])
{
	int col1 = mxGetN(in[0]);
	int row1 = mxGetM(in[0]);

	double *rawData1 = mxGetPr(in[0]);//coarse proposals

	double thr = *(double*)mxGetPr(in[1]);

	int i, j;
	double **data1;

	Box b;
	Boxes boxes, NMSBoxes;

	double *res = NULL;//Êä³ö½á¹û

	data1 = (double **)malloc(sizeof(double*)*row1);
	for (i = 0; i < row1; i++) data1[i] = (double *)malloc(sizeof(double)*col1);

	for (i = 0; i < row1; i++)for (j = 0; j<col1; j++) data1[i][j] = rawData1[i + j*row1];
	for (i = 0; i < row1; i++)
	{
		b.r = unsigned int(data1[i][1]);
		b.c = unsigned int(data1[i][0]);
		b.h = unsigned int(data1[i][3]);
		b.w = unsigned int(data1[i][2]);
		b.s = float(data1[i][4]);
		boxes.push_back(b);
	}

	boxesNms(boxes, float(thr), NMSBoxes);//coarseBoxes, superPixelBoxes, ratio

	out[0] = mxCreateDoubleMatrix(NMSBoxes.size(), col1 - 1, mxREAL);
	res = mxGetPr(out[0]);

	for (i = 0; i < NMSBoxes.size(); i++)
	{
		*(res + i + 0 * NMSBoxes.size()) = double(NMSBoxes[i].c);
		*(res + i + 1 * NMSBoxes.size()) = double(NMSBoxes[i].r);
		*(res + i + 2 * NMSBoxes.size()) = double(NMSBoxes[i].w);
		*(res + i + 3 * NMSBoxes.size()) = double(NMSBoxes[i].h);
	}


	for (i = 0; i < row1; i++) free(data1[i]);
	free(data1);
	return;
}