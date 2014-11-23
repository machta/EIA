#include "LinearSolverBase.h"

#ifndef GAUSSOPTIMIZEDSIMPLE_H
#define GAUSSOPTIMIZEDSIMPLE_H

class GaussOptimizedSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif