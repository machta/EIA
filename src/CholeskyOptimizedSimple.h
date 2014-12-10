#include "CholeskyTrivial.h"

#ifndef CHOLESKYOPTIMIZEDSIMPLE_H
#define CHOLESKYOPTIMIZEDSIMPLE_H

class CholeskyOptimizedSimple : public CholeskyTrivial
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif
