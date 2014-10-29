#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

#define A(a, b) A[a*n + b]

#ifndef CHOLESKYTRIVIAL_H
#define CHOLESKYTRIVIAL_H

class CholeskyTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n)
	{
		// Compute L, store it int A.		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < i; j++)
			{
				float sum = A(i, j);				
				for (int k = 0; k < j; k++)
					sum -= A(i, k)*A(j, k);
				
				A(i, j) = sum/A(j, j);
			}
			
			float sum = A(i, i);
			for (int k = 0; k < i; k++)
				sum -= A(i, k)*A(i, k);
			
			A(i, i) = sqrt(sum);
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
};

#endif