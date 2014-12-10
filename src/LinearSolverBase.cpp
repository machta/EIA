#include "LinearSolverBase.h"

#include <algorithm>

#include <omp.h>

using namespace std;

long long LinearSolverBase::performanceTest(int n)
{
	using namespace std::chrono;

	float* A;		
	int A0, N;
	allocateMatrix(&A, &A0, &N,  n);
	
	float* b = new float[n];
	float* x = new float[n];
	
	generateRandomSystem(A + A0, b, n, N);
	
	auto start = high_resolution_clock::now();
	
	solve(A + A0, b, x, n, N);
	
	auto end = high_resolution_clock::now();
	
	delete[] A; delete[] b; delete[] x;
	
	nanoseconds elapsedTime = end - start;
	
	return elapsedTime.count();
}

bool LinearSolverBase::correctnessTest(const char* file)
{
	int res;
	
	// Load the input.
	FILE* fs = fopen(file, "r");
	
	int n;
	res = fscanf(fs, "%d", &n);
	
	float* A;		
	int A0, N;
	allocateMatrix(&A, &A0, &N,  n);
	
	float* b = new float[n];
	float* x = new float[n];
	float* xCorrect = new float[n];
	
	A += A0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			res = fscanf(fs, "%f", &A(i, j));
		}		
	}
	A -= A0;
	
	for (int i = 0; i < n; i++)
	{
		res = fscanf(fs, "%f", b + i);
	}
	
	for (int i = 0; i < n; i++)
	{
		res = fscanf(fs, "%f", xCorrect + i);
	}
	(void)res;
	
	// Solve and check the result.
	solve(A + A0, b, x, n, N);
	
	int wrong = 0;
	for (int i = 0; i < n; i++)
	{
		bool res = almostEqual(x[i], xCorrect[i]);
		
		//fprintf(stderr, "a= %f b= %f diff= %f res= %d\n", x[i], xCorrect[i], fabs(x[i] - xCorrect[i]), res ? 1 : 0);
		
		if (res == false)
			wrong++;
	}
	
	delete[] A; delete[] b; delete[] x; delete[] xCorrect;
	fclose(fs);
	
	return wrong == 0;
}

void LinearSolverBase::generateRandomSystem(float* A, float* b, int n, int N)
{
	#define tmp(a, b) tmp[a*n + b]
	
	float* tmp = new float[n*n];
	
	// Fill A with random values.
	srand (time(NULL));
	const float maxRand = RAND_MAX/1000.;
	
	for (int i = 0; i < n; i++)
	{
		b[i] = rand()/maxRand - 500;
		
		for (int j = 0; j < n; j++)
		{
			tmp(i, j) = rand()/maxRand - 500;
		}
	}
	
	// Scale elements on the diagonal to prevent a system with no solution.
	for (int i = 0; i < n; i++)
	{
		tmp(i, i) += n;
	}
	
	// Shuffle the lines.
	int* rows = new int[n];
	for (int i = 0; i < n; i++)
	{
		rows[i] = i;
	}
	
	std::random_shuffle(rows, rows + n);
	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A(i, j) = tmp(rows[i], j);
		}
	}
	
	delete[] tmp; delete[] rows;
}

float LinearSolverBase::vectorDotProduct(float* A, float* B, int n)
{
	float sum = 0;
	__m128 sumV = _mm_set_ps1(0);
	
	int k = 0;
	int kn = min(n, (int)(nearestHigherAligned(A) - A));
	
	for (; k < kn; k++)
		sum += A[k]*B[k];
	
	kn += (n - kn)/4*4;
	__m128* Ap = (__m128*)(A + k);
	__m128* Bp = (__m128*)(B + k);
	
	for (; k < kn; k += 4)
	{
		sumV = _mm_add_ps(sumV, _mm_mul_ps(*Ap++, *Bp++));
	}
	
	for (; k < n; k++)
		sum += A[k]*B[k];
	
	for (int l = 0; l < 4; l++)
		sum += ((float*)&sumV)[l];
	
	return sum;
}

float LinearSolverBase::parallelVectorDotProduct(float* A, float* B, int n)
{
	float sum = 0;
	__m128 sumV = _mm_set_ps1(0);
	
	int k;
	int kn = min(n, (int)(nearestHigherAligned(A) - A));
	
	for (k = 0; k < kn; k++)
		sum += A[k]*B[k];
	
	kn = (n - kn)/4;
	__m128* Ap = (__m128*)(A + k);
	__m128* Bp = (__m128*)(B + k);
	
	//#pragma omp declare reduction (vectorSum : __m128 : omp_out = _mm_add_ps(omp_out, omp_in))
	
	//#pragma omp parallel for reduction(vectorSum : sumV)
	for (int i = 0; i < kn; i++)
	{
		sumV = _mm_add_ps(sumV, _mm_mul_ps(Ap[i], Bp[i]));
	}
	
	for (k += kn*4; k < n; k++)
		sum += A[k]*B[k];
	
	for (int l = 0; l < 4; l++)
		sum += ((float*)&sumV)[l];
	
	return sum;
}
