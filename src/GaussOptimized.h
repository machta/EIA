#include "LinearSolverBase.h"

using namespace std;

#ifndef GAUSSOPTIMIZEDUNROLLED_H
#define GAUSSOPTIMIZEDUNROLLED_H

class GaussOptimized : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif