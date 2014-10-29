#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

#define A(a, b) A[a*n + b]

#ifndef GAUSSTRIVIAL_H
#define GAUSSTRIVIAL_H

class GaussTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n)
	{
		//long long iters = 0;
		
		// Reduce A to lower triangular matrix.
		for (int i = 0; i < n; i++)
		{
			// Find the pivot.
			int pivot = i;
			for (int j = i + 1; j < n; j++)
			{
				if (fabs(A(j, i)) > fabs(A(pivot, i)))
					pivot = j;
			}
			
			// Exchange the rows.
			if (i != pivot)
			{
				for (int j = i; j < n; j++)
				{
					swap(A(i, j), A(pivot, j));
				}
				swap(b[i], b[pivot]);
			}
			
			// Eliminate the rest of the column.
			for (int j = i + 1; j < n; j++)
			{
				float tmp = A(j, i)/A(i, i);
				
				for (int k = i + 1; k < n; k++)
				{
					//iters++;
					
					A(j, k) -= A(i, k)*tmp;
				}
				b[j] -= b[i]*tmp;
			}
		}
		
		// Calculate x.
		for (int i = n - 1; i >= 0; i--)
		{
			float sum = 0;
			for (int j = i + 1; j < n; j++)
			{
				sum += A(i, j)*x[j];
			}
			x[i] = (b[i] - sum)/A(i, i);
		}
		
		//fprintf(stderr, "Gauss %d iters= %lld\n", n, iters);
	}
};

#endif