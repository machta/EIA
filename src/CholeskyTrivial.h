#include "LinearSolverBase.h"

#ifndef CHOLESKYTRIVIAL_H
#define CHOLESKYTRIVIAL_H

class CholeskyTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
	
	virtual void generateRandomSystem(float* A, float* b, int n, int N);
};

#endif