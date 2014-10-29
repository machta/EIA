#include "GaussTrivial.h"

#include "LUTrivial.h"

#include "CholeskyTrivial.h"

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

using namespace std;

void performanceTest(const vector<LinearSolverBase*>& tests, int testCount, int argc, char** argv)
{
	int maxOrder = 128;
	if (argc >= 3)
	{
		sscanf(argv[2], "%d", &maxOrder);
	}
	
	for (int i = 1; i <= maxOrder; i++)
	{
		printf("%5d ", i);
		
		for (int j = 0; j < testCount; j++)
		{
			long long time = tests[j]->performanceTest(i);
			printf("%12lld ", time);
		}
		
		printf("\n");
	}
}

void correctnessTest(const vector<LinearSolverBase*>& tests, int testCount, int argc, char** argv)
{
	vector<int> wrong(testCount, 0);
		
	// Print results for individual tests.
	for (int i = 2; i < argc; i++)
	{
		string str = string("                    ") + string(argv[i]);
		str = str.substr(str.size() - 20, 20);
		printf("%s ", str.c_str());
		
		for (int j = 0; j < testCount; j++)
		{
			bool result = tests[j]->correctnessTest(argv[i]);
			
			if (result)
			{
				printf("  OK ");
			}
			else
			{
				printf("FAIL ");
				wrong[j]++;
			}
		}
		
		printf("\n");
	}
	
	// Print the total.		
	printf("%s ", "         total fails");
	
	for (int j = 0; j < testCount; j++)
	{
		printf("%4d ", wrong[j]);
	}
	
	printf("\n");
}

/* Parameters:
 * mode (0 - performance test, 1 - correctness test)
 * max matrix order/test files
 */

int main(int argc, char** argv)
{
	// Process parameters.
	int mode = 0;
	
	if (argc >= 2)
	{
		sscanf(argv[1], "%d", &mode);
	}
	
	// Create test objects.
	const int testCount = 3;
	
	vector<LinearSolverBase*> tests;
	LinearSolverBase* t = new GaussTrivial; tests.push_back(t);
	t = new LUTrivial; tests.push_back(t);
	t = new CholeskyTrivial; tests.push_back(t);
	
	// Run tests.
	if (mode == 0)
	{
		performanceTest(tests, testCount, argc, argv);
	}
	else if (mode == 1)
	{
		correctnessTest(tests, testCount, argc, argv);
	}
	
	// Delete test objects.
	for (int i = 0; i < testCount; i++)
	{
		delete tests[i];
	}
}


