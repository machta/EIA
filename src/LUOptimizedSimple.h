#include "LinearSolverBase.h"

#ifndef LUOPTIMIZEDSIMPLE_H
#define LUOPTIMIZEDSIMPLE_H

class LUOptimizedSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif
