#include "GaussTrivial.h"
#include "GaussOptimizedSimple.h"
#include "GaussOptimized.h"
#include "GaussParallelSimple.h"

#include "LUTrivial.h"
#include "LUOptimizedSimple.h"
#include "LUParallelSimple.h"

#include "CholeskyTrivial.h"
#include "CholeskyOptimizedSimple.h"
#include "CholeskyParallelSimple.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

void performanceTest(const vector<LinearSolverBase*>& tests, int testCount, int argc, char** argv)
{
	int minOrder = 1;
	int maxOrder = 128;
	
	if (argc >= 4)
	{
		sscanf(argv[3], "%d", &minOrder);
	}
	
	if (argc >= 5)
	{
		sscanf(argv[4], "%d", &maxOrder);
	}
	
	for (int i = minOrder; i <= maxOrder; i++)
	{
		printf("%5d ", i);
		
		for (int j = 0; j < testCount; j++)
		{
			if (tests[j] != nullptr)
			{
				long long time = tests[j]->performanceTest(i);
				printf("%12lld ", time);
			}
			else
			{
				printf("           0 ");
			}
		}
		
		printf("\n");
	}
}

void correctnessTest(const vector<LinearSolverBase*>& tests, int testCount, int argc, char** argv)
{
	vector<int> wrong(testCount, 0);
		
	// Print results for individual tests.
	for (int i = 3; i < argc; i++)
	{
		string str = string("                    ") + string(argv[i]);
		str = str.substr(str.size() - 20, 20);
		printf("%s ", str.c_str());
		
		for (int j = 0; j < testCount; j++)
		{
			if (tests[j] != nullptr)
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
			else
			{
				printf("     ");
			}
		}
		
		printf("\n");
	}
	
	// Print the total.		
	printf("%s ", "         total fails");
	
	for (int j = 0; j < testCount; j++)
	{
		if (tests[j] != nullptr)
		{
			printf("%4d ", wrong[j]);
		}
		else
		{
			printf("     ");
		}
	}
	
	printf("\n");
}

int main(int argc, char** argv)
{
	// Create test objects.
	const int testCount = 10;
	
	vector<LinearSolverBase*> tests;
	
	tests.push_back(new GaussTrivial);
	tests.push_back(new LUTrivial);
	tests.push_back(new CholeskyTrivial);
	
	tests.push_back(new GaussOptimizedSimple);
	tests.push_back(new GaussOptimized);
	tests.push_back(new LUOptimizedSimple);
	tests.push_back(new CholeskyOptimizedSimple);
	
	tests.push_back(new GaussParallelSimple);
	tests.push_back(new LUParallelSimple);
	tests.push_back(new CholeskyParallelSimple);
	
	if (argc >= 2)
	{
		for (int i = 0; i < min(testCount, (int)strlen(argv[1])); i++)
		{
			if (argv[1][i] == '0')
			{
				delete tests[i];
				tests[i] = nullptr;
			}
		}
	}
	
	// Run tests.
	int mode = 0;
	
	if (argc >= 3)
	{
		sscanf(argv[2], "%d", &mode);
	}
	
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


