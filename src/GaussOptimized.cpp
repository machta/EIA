#include "GaussOptimized.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <algorithm>

#include <x86intrin.h>

using namespace std;

#define unrollFactor 4

// UNROLL_A
#if unrollFactor == 1
#define UNROLL_A \
	__m128 ratioV = _mm_set_ps1(ratios[j]);
#elif unrollFactor == 2
#define UNROLL_A \
	__m128 ratioV0 = _mm_set_ps1(ratios[j]);\
	__m128 ratioV1 = _mm_set_ps1(ratios[j + 1]);
#elif unrollFactor == 4
#define UNROLL_A \
	__m128 ratioV0 = _mm_set_ps1(ratios[j]);\
	__m128 ratioV1 = _mm_set_ps1(ratios[j + 1]);\
	__m128 ratioV2 = _mm_set_ps1(ratios[j + 2]);\
	__m128 ratioV3 = _mm_set_ps1(ratios[j + 3]);
#endif

// UNROLL_B
#if unrollFactor == 1
#define UNROLL_B \
	rowJ[k] = _mm_sub_ps(rowJ[k], _mm_mul_ps(rowI[k], ratioV));
#elif unrollFactor == 2
#define UNROLL_B \
	__m128 tmpI = rowI[k];\
	__m128* rowJ0 = &rowJ[k];\
	__m128* rowJ1 = rowJ0 + N/4;\
	*rowJ0 = _mm_sub_ps(*rowJ0, _mm_mul_ps(tmpI, ratioV0));\
	*rowJ1 = _mm_sub_ps(*rowJ1, _mm_mul_ps(tmpI, ratioV1));
#elif unrollFactor == 4
#define UNROLL_B \
	__m128 tmpI = rowI[k];\
	__m128* rowJ0 = &rowJ[k];\
	__m128* rowJ1 = rowJ0 + N/4;\
	__m128* rowJ2 = rowJ0 + 2*N/4;\
	__m128* rowJ3 = rowJ0 + 3*N/4;\
	*rowJ0 = _mm_sub_ps(*rowJ0, _mm_mul_ps(tmpI, ratioV0));\
	*rowJ1 = _mm_sub_ps(*rowJ1, _mm_mul_ps(tmpI, ratioV1));\
	*rowJ2 = _mm_sub_ps(*rowJ2, _mm_mul_ps(tmpI, ratioV2));\
	*rowJ3 = _mm_sub_ps(*rowJ3, _mm_mul_ps(tmpI, ratioV3));
#endif

/*
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
*/

GaussOptimized::GaussOptimized()
{
	jTile = 8;
	kTile = 32;
	
	char line[100];
	FILE* file = fopen("tileOptions", "r");
	
	fgets(line, 100, file);
	sscanf(line, "%d", &jTile);
	
	fgets(line, 100, file);
	sscanf(line, "%d", &kTile);
	
	fclose(file);
}

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
		
		//const int jTile = 8; // Moved to a file; read in the constructor.
		//const int kTile = 32;
		//const int unrollFactor = 2; // Moved to the top of this file.

		__m128* rowI = reinterpret_cast<__m128*>(&A(i, vectorStart));
		int kkn = (N - vectorStart)/4;

		for (int jj = i + 1; jj < n; jj += jTile)
		{
			for (int kk = 0; kk < kkn; kk += kTile)
			{
				// Vectorized, unrolled.
				int jn = min(n, jj + jTile);				
				int jnU = jj + (jn - jj)/unrollFactor*unrollFactor;
				
				for (int j = jj; j < jnU; j += unrollFactor)
				{
					__m128* rowJ = rowI + (j - i)*N/4;
					
					UNROLL_A;
					
					int kn = min(kkn, kk + kTile);
					for (int k = kk; k < kn; k++)
					{
						UNROLL_B;
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
