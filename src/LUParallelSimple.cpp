#include "LUParallelSimple.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <algorithm>

#include <omp.h>

using namespace std;

#define U(i, j) U[U0 + (j)*UN + (i)]

void LUParallelSimple::solve(float* A, float* b, float* x, int n, int N)
{
	// A holds all elements from L plus the diagonal from U.
	// U is in column-major order.

	int* index = new int[n];
	float* rowNormalizer = new float[n];

	float* U;
	int U0, UN;
	allocateMatrix(&U, &U0, &UN, n);
	assert(UN == N);

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
		for (int i = 0; i < j; i++)
		{
			// Evaluate U.
			float sum = parallelVectorDotProduct(&A(i, 0), &U(0, j), i);
	
			U(i, j) = A(i, j) - sum;
		}

		// Find pivot element.
		int pivotRow = -1;
		float maxvalue = 0;
		for (int i = j; i < n; i++)
		{
			// Evaluate L.
			float sum = parallelVectorDotProduct(&A(i, 0), &U(0, j), j);
	
			sum = A(i, j) - sum;
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
				swap(U(j, k), U(pivotRow, k));
			}
	
			swap(index[j], index[pivotRow]);				
			rowNormalizer[pivotRow] = rowNormalizer[j];
		}

		// Divide by pivot element.
		float denom = 1/A(j, j);
		for (int i = j + 1; i < n; i++)
			A(i, j) *= denom;
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
			sum -= U(i, k)*x[k];
		x[i] = sum/A(i, i);
	}

	delete[] index;
	delete[] rowNormalizer;
	delete[] U;
}

