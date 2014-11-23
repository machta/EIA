#include "LinearSolverBase.h"

#ifndef LUOPTIMIZEDSIMPLE_H
#define LUOPTIMIZEDSIMPLE_H

class LUOptimizedSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
	
private:
	float vectorSum(int i, int j, int elements, float* A, int N, float* U, int UN, int U0);
};

#endif