#include "mex.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "stdlib.h"
#include <assert.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <strstream>
using namespace std;


#define CHK_IND(p) ((p).x >= 0 && (p).x < _w && (p).y >= 0 && (p).y < _h)

/************************************************************************/
/* A value struct vector that supports efficient sorting                */
/************************************************************************/

template<typename VT, typename ST> 
struct ValStructVec
{
	ValStructVec(){clear();}
	inline int size() const {return sz;} 
	inline void clear() {sz = 0; structVals.clear(); valIdxes.clear();}
	inline void reserve(int resSz){clear(); structVals.reserve(resSz); valIdxes.reserve(resSz); }
	inline void pushBack(const VT& val, const ST& structVal) {valIdxes.push_back(make_pair(val, sz)); structVals.push_back(structVal); sz++;}

	inline const VT& operator ()(int i) const {return valIdxes[i].first;} // Should be called after sort
	inline const ST& operator [](int i) const {return structVals[valIdxes[i].second];} // Should be called after sort
	inline VT& operator ()(int i) {return valIdxes[i].first;} // Should be called after sort
	inline ST& operator [](int i) {return structVals[valIdxes[i].second];} // Should be called after sort

	void sort(bool descendOrder = true);
	const vector<ST> &getSortedStructVal();
	void append(const ValStructVec<VT, ST> &newVals, int startV = 0);

	vector<ST> structVals; // struct values
	vector<pair<VT, int>> valIdxes; // Indexes after sort
private:
	int sz; // size of the value struct vector
	
	bool smaller() {return true;};
	vector<ST> sortedStructVals; 
};

template<typename VT, typename ST> 
void ValStructVec<VT, ST>::append(const ValStructVec<VT, ST> &newVals, int startV)
{
	int sz = newVals.size();
	for (int i = 0; i < sz; i++)
		pushBack((float)((i+300)*startV)/*newVals(i)*/, newVals[i]);
}

template<typename VT, typename ST> 
void ValStructVec<VT, ST>::sort(bool descendOrder /* = true */)
{
	if (descendOrder)
		std::sort(valIdxes.begin(), valIdxes.end(), std::greater<pair<VT, int>>());
	else
		std::sort(valIdxes.begin(), valIdxes.end(), std::less<pair<VT, int>>());
}

template<typename VT, typename ST> 
const vector<ST>& ValStructVec<VT, ST>::getSortedStructVal()
{
	sortedStructVals.resize(sz);
	for (int i = 0; i < sz; i++)
		sortedStructVals[i] = structVals[valIdxes[i].second];
	return sortedStructVals;
}

struct PointXY
{
	unsigned int x;
	unsigned int y;
};

void nonMaxSup(double **matchCost1f, int row, int col, ValStructVec<float, PointXY> &matchCost, int NSS, int maxPoint, double Rate, unsigned char step)
{
	const int _h = row, _w = col;
	unsigned int i = 0, j = 0;
	unsigned int r = 0, c = 0;
	int dx = 0 ,dy = 0;
	unsigned int count = 0;
	unsigned char *isMax1u = (unsigned char *)malloc(sizeof(unsigned char) * row * col);
	ValStructVec<double, PointXY> valPnt;
	matchCost.reserve(_h * _w);
	valPnt.reserve(_h * _w);
	PointXY p;
	PointXY neighbor;
	PointXY pnt;

	unsigned int m = 0, n = 0;
	double tmp = 0;
	
	double **matchCost1fMean = (double **)malloc(sizeof(double)*(row+2*step));
    for(i = 0; i < row+2*step; i++)
         matchCost1fMean[i] = (double *)malloc(sizeof(double)*(col+2*step));

	for (i = 0; i < row; i++)for(j = 0; j <col; j++)isMax1u[i*col + j] = 1;
	//add edge
	for (i = 1; i < row+2*step; i++)for(j = 1; j <col+2*step; j++)matchCost1fMean[i][j] = 0;
	for (i = step; i < row+step; i++)for(j = step; j <col+step; j++)matchCost1fMean[i][j] = matchCost1f[i-step][j-step];
	for (i = 0; i < step; i++)for(j = 0; j < step; j++)
		{
			matchCost1fMean[i][j] = matchCost1f[0][0];
			matchCost1fMean[row+step+i][j] = matchCost1f[row-1][0];
			matchCost1fMean[i][col+step+j] = matchCost1f[0][col-1];
			matchCost1fMean[row+step+i][col+step+j] = matchCost1f[row-1][col-1];

		}

	for (i = 0; i < row; i++)for(j = 0; j <step; j++)
		{
			matchCost1fMean[i+step][j] = matchCost1f[i][0];
			matchCost1fMean[i+step][col+1+j] = matchCost1f[i][col-1];
		}

	for (i = 0; i < col; i++)for(j = 0; j <step; j++)
		{
			matchCost1fMean[j][i+step] = matchCost1f[0][i];
			matchCost1fMean[row+1+j][i+step] = matchCost1f[row-1][i];
		}

	for (i = 1; i < row+1; i++)for(j = 1; j <col+1; j++)
		{
			tmp = 0;
			for(m = -step; m <= step; m++)for(n = -step; n <= step; n++)
				tmp += matchCost1fMean[i+m][j+n];
			matchCost1fMean[i][j] = tmp/((2*step+1)*(2*step+1));
		}


	for (r = 0; r < _h; r++){
		for (c = 0; c < _w; c++)
		{
			if(matchCost1f[r][c] >= matchCost1fMean[r+1][c+1])
			{
				p.x = c;
				p.y = r;
				valPnt.pushBack(matchCost1f[r][c], p);
			}
		}
	}

	valPnt.sort();
	for (i = 0; i < valPnt.size(); i++){
		
		pnt.x= valPnt[i].x;
		pnt.y= valPnt[i].y;
		if (isMax1u[pnt.y*col+pnt.x]){
			matchCost.pushBack(valPnt(i), pnt);
			for (dy = -NSS; dy <= NSS; dy++) for (dx = -NSS; dx <= NSS; dx++){				
				neighbor.x = pnt.x + dx;
				neighbor.y = pnt.y + dy;
				if (!(neighbor.x >= 0 && neighbor.x < _w && neighbor.y >= 0 && neighbor.y < _h))
					continue;
				if (isMax1u[neighbor.y*col+neighbor.x] == 1)
				{
					isMax1u[neighbor.y*col+neighbor.x] = 0;
					count++;
				}
			}
		}
		if (count >= Rate * row* col || matchCost.size() >= maxPoint)
		{
			free(isMax1u);
			for (i = 0; i < row + 2 * step; i++)
				free(matchCost1fMean[i]);
			free(matchCost1fMean);
			return;
		}
	}
	free(isMax1u);
	for(i = 0; i < row+2*step;i++)
		free(matchCost1fMean[i]);
	free(matchCost1fMean);
}

void mexFunction(int nout, mxArray *out[], 	int nin, const mxArray *in[])
{
	int col = mxGetN(in[0]);
	int row = mxGetM(in[0]);


	double *data = mxGetPr(in[0]);

	double NSS = *(double*)mxGetPr(in[1]);
	double NUM_WIN_PSZ = *(double*)mxGetPr(in[2]);
	double Rate = *(double*)mxGetPr(in[3]);
	double RatioH = *(double*)mxGetPr(in[4]);
	double RatioW = *(double*)mxGetPr(in[5]);

	int i,j;
	double **matchCost1f;
	unsigned char step = 1;

	double *res = NULL;//Êä³ö½á¹û
	ValStructVec<float, PointXY> matchCost;
	matchCost1f = (double **)malloc(sizeof(double)*row);
    for(i = 0; i < row; i++)
         matchCost1f[i] = (double *)malloc(sizeof(double)*col);

	for (i=0;i<row ;i++)for (j=0;j<col;j++)matchCost1f[i][j] = data[i+j*row];

	nonMaxSup(matchCost1f,row,col, matchCost, NSS, NUM_WIN_PSZ, Rate, step);

	out[0] =  mxCreateDoubleMatrix(3,matchCost.size(),mxREAL);
	res = mxGetPr(out[0]);
	for(i = 0 ; i < matchCost.size(); i++)
	{
		*(res+3*i+0) = matchCost.valIdxes[i].first;
		*(res+3*i+1) = matchCost.structVals[i].y+1;
		*(res+3*i+2) = matchCost.structVals[i].x+1;
	}
	for(i = 0; i < row;i++)
		free(matchCost1f[i]);
	free(matchCost1f);
return;		
}

