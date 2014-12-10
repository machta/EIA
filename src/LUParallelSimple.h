#include "LinearSolverBase.h"

#ifndef LUPARALLELSIMPLE_H
#define LUPARALLELSIMPLE_H

class LUParallelSimple : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif
