#include "GaussOptimizedSimple.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

#include <x86intrin.h>

using namespace std;

void GaussOptimizedSimple::solve(float* A, float* b, float* x, int n, int N)
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
		
		// Process the first few elements so that the vectorized loop can start on a 16-byte aligned address.
		int vectorStart = i + 1 + (nearestHigherAligned(&A(i, i + 1)) - &A(i, i + 1));
		vectorStart = min(vectorStart, n);
		
		for (int j = i + 1; j < n; j++)
		{
			float ratio = A(j, i)/A(i, i);
			
			for (int k = i + 1; k < vectorStart; k++)
			{
				A(j, k) -= A(i, k)*ratio;
			}
			
			b[j] -= b[i]*ratio;
		}
		
		// Vectorirized.
		__m128* rowI = reinterpret_cast<__m128*>(&A(i, vectorStart));
		int kn = (N - vectorStart)/4;
		
		for (int j = i + 1; j < n; j++)
		{
			__m128* rowJ = rowI + (j - i)*N/4;
			__m128 ratio = _mm_set_ps1(A(j, i)/A(i, i));
			
			for (int k = 0; k < kn; k++)
			{
				rowJ[k] = _mm_sub_ps(rowJ[k], _mm_mul_ps(rowI[k], ratio));
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
	
	//fprintf(stderr, "Gauss %d iters= %lld\n", n, iters);
}
