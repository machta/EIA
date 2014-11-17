#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

#include <x86intrin.h>

using namespace std;

#ifndef GAUSSOPTIMIZEDUNROLLED_H
#define GAUSSOPTIMIZEDUNROLLED_H

class GaussOptimizedUnrolled : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N, int A0)
	{
		//long long iters = 0;
		
		float* ratios = new float[n];
		
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
			
			// Process until 64-byte aligned address.
			int rowOffset = nearestHigherAligned(&A(i, i + 1), 64) - &A(i, i + 1);
			rowOffset += i + 1;
			
			for (int j = i + 1; j < n; j++)
			{
				ratios[j] = A(j, i)/A(i, i); // ratio
				
				int lastK = min(rowOffset, n);
				for (int k = i + 1; k < lastK; k++)
				{
					A(j, k) -= A(i, k)*ratios[j];
				}
				b[j] -= b[i]*ratios[j];
			}
			
			rowOffset += 16;
			
			// Vectorized and unrolled.
			__m128 rowI0, rowI1, rowI2, rowI3;
			
			const int jjStep = 16;
			int rowStart = rowOffset;
			
			for (int jj = i + 1; jj < n; jj += jjStep)
			{
				for (rowOffset = rowStart ; rowOffset <= n; rowOffset += 16)
				{
					rowI0 = _mm_load_ps(&A(i, rowOffset - 16));
					rowI1 = _mm_load_ps(&A(i, rowOffset - 12));
					rowI2 = _mm_load_ps(&A(i, rowOffset - 8));
					rowI3 = _mm_load_ps(&A(i, rowOffset - 4));
					
					for (int j = jj; j < min(n, jj + jjStep); j++)
					{
						__m128 ratio = _mm_set_ps1(ratios[j]);
						__m128 tmp;
						__m128* rowJ = reinterpret_cast<__m128*>(&A(j, rowOffset - 16));
						
						tmp = _mm_mul_ps(rowI0, ratio);
						*rowJ = _mm_sub_ps(*rowJ, tmp);
						rowJ++;
						
						tmp = _mm_mul_ps(rowI1, ratio);
						*rowJ = _mm_sub_ps(*rowJ, tmp);
						rowJ++;
						
						tmp = _mm_mul_ps(rowI2, ratio);
						*rowJ = _mm_sub_ps(*rowJ, tmp);
						rowJ++;
						
						tmp = _mm_mul_ps(rowI3, ratio);
						*rowJ = _mm_sub_ps(*rowJ, tmp);
					}
				}
			}
			
			rowOffset -= 16;
			
			// This part is taken from GaussOptimizedSimple.
			//const int colWidth = L1/sizeof(__m128)/3;
			//__m128* rowI = (__m128*)&A(i, i + 1 + rowOffset);
			__m128* rowI = (__m128*)&A(i, rowOffset);
			
			//while (rowI < (__m128*)&A(i + 1, 0))
			//{
				//int kn = min(rowI + colWidth, (__m128*)&A(i + 1, 0)) - rowI;
				int kn = (__m128*)&A(i + 1, 0) - rowI;
				
				for (int j = i + 1; j < n; j++)
				{
					__m128* rowJ = rowI + (j - i)*N/4;
					//__m128 ratio = _mm_set_ps1(A(j, i));
					__m128 ratio = _mm_set_ps1(ratios[j]);
					
					for (int k = 0; k < kn; k++)
					{
						__m128 tmp = _mm_mul_ps(rowI[k], ratio);
						rowJ[k] = _mm_sub_ps(rowJ[k], tmp);
					}
				}
				
				//rowI += kn;
			//}
			
			/*for (int j = i + 1; j < n; j++)
			{
				for (int k = rowOffset; k < n; k++)
				{
					A(j, k) -= A(i, k)*ratios[j];
				}
			}*/
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
		
		delete[] ratios;
		
		//fprintf(stderr, "Gauss %d iters= %lld\n", n, iters);
	}
};

#endif