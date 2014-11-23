#include "LinearSolverBase.h"

#ifndef GAUSSTRIVIAL_H
#define GAUSSTRIVIAL_H

class GaussTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif