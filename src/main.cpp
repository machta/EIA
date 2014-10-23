#include "GaussTrivial.h"

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

using namespace std;

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
	
	// Run tests.
	if (mode == 0) // Performance test.
	{
		int maxOrder = 128;
		if (argc >= 3)
		{
			sscanf(argv[2], "%d", &maxOrder);
		}
		
		
	}
	else if (mode == 1) // Correctness/precision test.
	{
		const int testCount = 1;
		
		vector<int> wrong(testCount, 0);
		
		vector<LinearSolverBase*> tests;
		GaussTrivial* gt = new GaussTrivial; tests.push_back(gt);
		gt = new GaussTrivial; tests.push_back(gt);
		
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
		
		// Delete tests.
		for (int i = 0; i < testCount; i++)
		{
			delete tests[i];
		}
	}
}


