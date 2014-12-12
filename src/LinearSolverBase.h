#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstdint>

#include <chrono>
#include <algorithm>

#ifndef LINEARSOLVERBASE_H
#define LINEARSOLVERBASE_H

#define A(i, j) A[(i)*N + (j)]

class LinearSolverBase
{
public:
	virtual ~LinearSolverBase() {}
	
	long long performanceTest(int n);
	
	bool correctnessTest(const char* file);
	
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N) = 0;
	
	virtual void generateRandomSystem(float* A, float* b, int n, int N);
	
	template <class T>
	T* nearestHigherAligned(T* ptr, int alignTo = 16)
	{
		uintptr_t tmp = reinterpret_cast<uintptr_t>(ptr);
		
		alignTo--;
		tmp = (tmp + alignTo) & ~alignTo;
		
		return reinterpret_cast<T*>(tmp);
	}
	
	void allocateMatrix(float** A, int* A0, int* N, int n)
	{
		// Ensure that each row is 16-byte aligned.
		*N = (n + 3) & ~3;
		
		*A = new float[*N*n + 3];
		
		*A0 = nearestHigherAligned(*A) - *A;
		
		for (int i = 0; i < (*N*n + 3); i++)
			(*A)[i] = 0;
	}
	
	float vectorDotProduct(float* A, float* B, int n);
	float parallelVectorDotProduct(float* A, float* B, int n);
	
private:
	bool almostEqual(float A, float B, float maxAbsoluteError = 0.0001, float maxRelativeError = 0.01)
	{
		if (fabs(A - B) < maxAbsoluteError)
			return true;

		float relativeError;
		if (fabs(B) > fabs(A))
			relativeError = fabs((A - B) / B);
		else
			relativeError = fabs((A - B) / A);

		return relativeError <= maxRelativeError;
	}
};

#endif
