#include "CholeskyTrivial.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

#ifndef CHOLESKYOPTIMIZEDSIMPLE_H
#define CHOLESKYOPTIMIZEDSIMPLE_H

class CholeskyOptimizedSimple : public CholeskyTrivial
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N, int A0)
	{	
		// Compute L, store it int A.
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < i; j++)
			{
				/*float sum = A(i, j);				
				for (int k = 0; k < j; k++)
				{
					sum -= A(i, k)*A(j, k);
				}*/
				
				float sum = vectorSum(i, j, j, A, N, A0);
				
				A(i, j) = (A(i, j) - sum)/A(j, j);
			}
			
			/*float sum = A(i, i);			
			for (int k = 0; k < i; k++)
			{
				sum -= A(i, k)*A(i, k);
			}*/
			
			float sum = vectorSum(i, i, i, A, N, A0);
			
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
	
private:
	float vectorSum(int i, int j, int elements, float* A, int N, int A0)
	{
		float sum = 0;
		__m128 sumV = _mm_set_ps1(0);
		
		int k = 0;
		int kn = min(elements, nearestHigherAligned(&A(i, 0)) - &A(i, 0));
		
		for (; k < kn; k++)
			sum += A(i, k)*A(j, k);
		
		kn += (elements - kn)/4*4;
		__m128* Ip = (__m128*)&A(i, k);
		__m128* Jp = (__m128*)&A(j, k);
		
		for (; k < kn; k += 4)
		{
			sumV = _mm_add_ps(sumV, _mm_mul_ps(*Ip++, *Jp++));
		}
		
		for (; k < elements; k++)
			sum += A(i, k)*A(j, k);
		
		for (int l = 0; l < 4; l++)
			sum += ((float*)&sumV)[l];
		
		return sum;
	}
};

#endif