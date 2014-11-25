#include "GaussOptimized.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

#include <x86intrin.h>

using namespace std;

void GaussOptimized::solve(float* A, float* b, float* x, int n, int N)
{
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
		
		// Process the first few elements so that the vectorized loop can start on a 16-byte aligned address.
		int vectorStart = i + 1 + (nearestHigherAligned(&A(i, i + 1)) - &A(i, i + 1));
		vectorStart = min(vectorStart, n);
		
		for (int j = i + 1; j < n; j++)
		{
			ratios[j] = A(j, i)/A(i, i);
			
			for (int k = i + 1; k < vectorStart; k++)
			{
				A(j, k) -= A(i, k)*ratios[j];
			}
			
			b[j] -= b[i]*ratios[j];
		}
		
		// Optimized loop.
		const int jTile = 16;
		const int kTile = 64;
		const int unrollFactor = 4;

		__m128* rowI = reinterpret_cast<__m128*>(&A(i, vectorStart));
		int kkn = (N - vectorStart)/4;

		for (int jj = i + 1; jj < n; jj += jTile)
		{
			for (int kk = 0; kk < kkn; kk += kTile)
			{
				int jn = min(n, jj + jTile);

				// Unrolled.
				int jnU = jj + (jn - jj)/unrollFactor*unrollFactor;
				for (int j = jj; j < jnU; j += unrollFactor)
				{
					__m128 ratio[unrollFactor];
					for (int l = 0; l < unrollFactor; l++)
					{
						ratio[l] = _mm_set_ps1(ratios[j + l]);
					}
					
					int kn = min(kkn, kk + kTile);
					for (int k = kk; k < kn; k++)
					{
						__m128 tmp = rowI[k];
						for (int l = 0; l < unrollFactor; l++)
						{
							__m128* rowJ = rowI + k + (j + l - i)*N/4;
							*rowJ = _mm_sub_ps(*rowJ, _mm_mul_ps(tmp, ratio[l]));
						}
					}
				}

				for (int j = jnU; j < jn; j++)
				{
					__m128* rowJ = rowI + (j - i)*N/4;
					__m128 ratio = _mm_set_ps1(ratios[j]);
					
					int kn = min(kkn, kk + kTile);
					for (int k = kk; k < kn; k++)
					{
						rowJ[k] = _mm_sub_ps(rowJ[k], _mm_mul_ps(rowI[k], ratio));
					}
				}
			}
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
	
	delete[] ratios;
}
