#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

#include <x86intrin.h>

using namespace std;

#ifndef GAUSSOPTIMIZEDSIMPLE_H
#define GAUSSOPTIMIZEDSIMPLE_H

class GaussOptimizedSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N, int A0)
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
			int rowOffset = nearestHigher16BAligned(&A(i, i + 1)) - &A(i, i + 1);
			
			for (int j = i + 1; j < n; j++)
			{
				A(j, i) = A(j, i)/A(i, i); // ratio
				
				for (int k = i + 1; k < i + 1 + rowOffset; k++)
				{
					A(j, k) -= A(i, k)*A(j, i);
				}
				b[j] -= b[i]*A(j, i);
			}
			
			const int colWidth = L1/sizeof(__m128)/3;
			__m128* rowI = (__m128*)&A(i, i + 1 + rowOffset);
			
			while (rowI < (__m128*)&A(i + 1, 0))
			{
				int kn = min(rowI + colWidth, (__m128*)&A(i + 1, 0)) - rowI;
				
				for (int j = i + 1; j < n; j++)
				{
					__m128* rowJ = rowI + (j - i)*N/4;
					__m128 ratio = _mm_set_ps1(A(j, i));
					
					for (int k = 0; k < kn; k++)
					{
						__m128 tmp = _mm_mul_ps(rowI[k], ratio);
						rowJ[k] = _mm_sub_ps(rowJ[k], tmp);
					}
				}
				
				rowI += kn;
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