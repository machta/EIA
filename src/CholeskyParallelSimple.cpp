#include "CholeskyParallelSimple.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

void CholeskyParallelSimple::solve(float* A, float* b, float* x, int n, int N)
{
	// Compute L, store it int A.
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			float sum = parallelVectorDotProduct(&A(i, 0), &A(j, 0), j);
		
			A(i, j) = (A(i, j) - sum)/A(j, j);
		}
	
		float sum = parallelVectorDotProduct(&A(i, 0), &A(i, 0), i);
	
		A(i, i) = sqrt(A(i, i) - sum);
	}
	
	// Compute x.
	for (int i = 0; i < n; i++)
	{
		float sum = 0;
		for (int j = 0; j < i; j++)
		{
			sum += A(i, j)*x[j];
		}
		x[i] = (b[i] - sum)/A(i, i);
	}

	for (int i = n - 1; i >= 0; i--)
	{
		float sum = 0;
		for (int j = i + 1; j < n; j++)
		{
			sum += A(j, i)*x[j];
		}
		x[i] = (x[i] - sum)/A(i, i);
	}
}

