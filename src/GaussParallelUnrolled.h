#include "LinearSolverBase.h"

using namespace std;

#ifndef GAUSSPARALLELUNROLLED_H
#define GAUSSPARALLELUNROLLED_H

class GaussParallelUnrolled : public LinearSolverBase
{
public:
	GaussParallelUnrolled();
	
protected:
	virtual void solve(float* A, float* b, float* x, int n, int N);
	
private:
	int jTile, kTile;
};

#endif