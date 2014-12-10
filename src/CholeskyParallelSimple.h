#include "CholeskyTrivial.h"

#ifndef CHOLESKYPARALLELSIMPLE_H
#define CHOLESKYPARALLELSIMPLE_H

class CholeskyParallelSimple : public CholeskyTrivial
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif
