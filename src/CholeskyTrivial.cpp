#include "CholeskyTrivial.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

void CholeskyTrivial::solve(float* A, float* b, float* x, int n, int N)
{
	// Compute L, store it int A.		
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			float sum = A(i, j);
			
			for (int k = 0; k < j; k++)
			{
				sum -= A(i, k)*A(j, k);
			}
			
			A(i, j) = sum/A(j, j);
		}
		
		float sum = A(i, i);
		
		for (int k = 0; k < i; k++)
		{
			sum -= A(i, k)*A(i, k);
		}
		
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

void CholeskyTrivial::generateRandomSystem(float* A, float* b, int n, int N)
{
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
