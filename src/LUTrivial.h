#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

#define A(a, b) A[a*n + b]

#ifndef LUTRIVIAL_H
#define LUTRIVIAL_H

class LUTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n)
	{
		int* index = new int[n];
		float* rowNormalizer = new float[n];
		
		// Calculate a normalizer for each row.
		for (int i = 0; i < n; i++)
		{
			float maxvalue = 0.0F;
			for (int j = 0; j < n; j++)
			{
				float value = fabs(A(i, j));
				if (value > maxvalue)
					maxvalue = value;
			}
			
			rowNormalizer[i] = 1/maxvalue;
			index[i] = i;
		}
		
		// Perform decomposition.
		for (int j = 0; j < n; j++)
		{
			for (int i = 1; i < j; i++)
			{
				// Evaluate U.
				float sum = A(i, j);
				for (int k = 0; k < i; k++)
					sum -= A(i, k)*A(k, j);
				A(i, j) = sum;
			}
			
			// Find pivot element.
			int pivotRow = -1;
			float maxvalue = 0;
			for (int i = j; i < n; i++)
			{
				// Evaluate L.
				float sum = A(i, j);
				for (int k = 0; k < j; k++)
					sum -= A(i, k)*A(k, j);
				A(i, j) = sum;
				
				sum = fabs(sum)*rowNormalizer[i];
				if (sum > maxvalue)
				{
					maxvalue = sum;
					pivotRow = i;
				}
			}
			
			if (pivotRow != j)
			{
				// Exchange rows.
				for (int k = 0; k < n; k++)
				{
					swap(A(j, k), A(pivotRow, k));
				}
				
				swap(index[j], index[pivotRow]);				
				rowNormalizer[pivotRow] = rowNormalizer[j];
			}
			
			// Divide by pivot element.
			if (j != n - 1)
			{
				float denom = 1/A(j, j);
				for (int i = j + 1; i < n; i++)
					A(i, j) *= denom;
			}
		}
		
		// Perform forward substitution for Ly = b.
		for (int i = 0; i < n; i++)
		{
			float sum = b[index[i]];
			for (int k = 0; k < i; k++)
				sum -= A(i, k)*x[k];
			x[i] = sum;
		}
		
		// Perform backward substitution for Ux = y.
		for (int i = n - 1; i >= 0; i--)
		{
			float sum = x[i];
			for (int k = i + 1; k < n; k++)
				sum -= A(i, k)*x[k];
			x[i] = sum/A(i, i);
		}
		
		delete[] index;
		delete[] rowNormalizer;
	}
};

#endif