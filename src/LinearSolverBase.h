#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <limits>

#include <chrono>
#include <algorithm>

#ifndef LINEARSOLVERBASE_H
#define LINEARSOLVERBASE_H

#define A(a, b) A[a*N + b]

class LinearSolverBase
{
public:
	virtual ~LinearSolverBase()
	{}
	
	long long performanceTest(int n)
	{
		using namespace std::chrono;

		float* A;		
		int N = allocateMatrix(&A, n);
		
		float* b = new float[n];
		float* x = new float[n];
		
		generateRandomSystem(A, b, n, N);
		
		auto start = high_resolution_clock::now();
		
		solve(A, b, x, n, N);
		
		auto end = high_resolution_clock::now();
		
		delete[] A; delete[] b; delete[] x;
		
		nanoseconds elapsedTime = end - start;
		
		return elapsedTime.count();
	}
	
	bool correctnessTest(const char* file)
	{
		int res;
		
		// Load the input.
		FILE* fs = fopen(file, "r");
		
		int n;
		res = fscanf(fs, "%d", &n);
		
		float* A;		
		int N = allocateMatrix(&A, n);
		
		float* b = new float[n];
		float* x = new float[n];
		float* xCorrect = new float[n];
		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				res = fscanf(fs, "%f", &A(i, j));
			}		
		}
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
		solve(A, b, x, n, N);
		
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
	
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N) = 0;
	
	virtual void generateRandomSystem(float* A, float* b, int n, int N)
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
	
	int allocateMatrix(float** A, int n)
	{
		// Ensure that each row is 16-byte aligned.
		int N = (n + 3) & ~3;
		
		*A = new float[N*n];
		
		for (int i = 0; i < N*n; i++)
			(*A)[i] = 0;
		
		return N;
	}
};

#endif