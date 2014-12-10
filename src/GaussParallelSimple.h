#include "LinearSolverBase.h"

#ifndef GAUSSPARALLELSIMPLE_H
#define GAUSSPARALLELSIMPLE_H

class GaussParallelSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif
