#include "LinearSolverBase.h"

#ifndef LUTRIVIAL_H
#define LUTRIVIAL_H

class LUTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
};

#endif