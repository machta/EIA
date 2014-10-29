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
		//long long iters = 0;
		
		// Compute L, store it int A.		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < i; j++)
			{
				float sum = A(i, j);
				
				for (int k = 0; k < j; k++)
				{
					//iters++;
					
					sum -= A(i, k)*A(j, k);
				}				
				/*float* rowI = &A(i, 0);
				float* rowJ = &A(j, 0);
				for (int k = 0; k < j; k++)
				{
					//iters++;
					
					sum -= (*rowI++)*(*rowJ++);
				}*/
				
				A(i, j) = sum/A(j, j);
			}
			
			float sum = A(i, i);
			
			for (int k = 0; k < i; k++)
			{
				sum -= A(i, k)*A(i, k);
			}
			/*float* rowI = &A(i, 0);
			for (int k = 0; k < i; k++)
			{
				float tmp = *rowI++;
				sum -= tmp*tmp;
			}*/
			
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
		
		//fprintf(stderr, "Cholesky %d iters= %lld\n", n, iters);
	}
	
	virtual void generateRandomSystem(float* A, float* b, int n)
	{
		#define A(a, b) A[a*n + b]
		
		// Fill A with random values.
		srand (time(NULL));
		const float maxRand = RAND_MAX;
		
		for (int i = 0; i < n; i++)
		{
			b[i] = rand()/maxRand;
			
			for (int j = 0; j <= i; j++)
			{
				A(i, j) = A(j, i) = rand()/maxRand;
			}
		}
		
		// Scale elements on the diagonal to prevent a system with no solution.
		for (int i = 0; i < n; i++)
		{
			A(i, i) += n;
		}
	}
};

#endif