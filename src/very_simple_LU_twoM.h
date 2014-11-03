#include "LinearSolverBase.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <algorithm>

using namespace std;

#define A(a, b) A[a*n + b]

#ifndef LUVERYTRIVIAL_H
#define LUVERYTRIVIAL_H
//moc jednoduchy LU rozklad do dvou matic bez pivotizace
class LUVeryTrivial : public LinearSolverBase
{
protected:
	virtual void solve(float* A, float* b, float* x, int n)
	{
	
		float** l=new float *[n];
    float ** u =new float *[n];
		
     for (int i = 0; i < n; i++) {
        l[i] = new float[size];
        u[i] = new float[size];
        
    }
		
    //prvni radek u matice je roven prvnimu radku A
    //vyplnime 1. radek u a l matic
    for (int j = 0; j < n; j++) {
        u[0][j] = A(0,j); //clen horni trojuhelnikova matice U      
    }
    //l matice ma na diagoale 1
    l[0][0] = 1;
  
    //provedeme LU rozklad, vysledky budou uchovany do 2 matic
    for (int i = 1; i < n; i++) {
        //spocitame cast L matice - potrebujeme ji pro U

        for (int j = 0; j < i; j++) {
            float sum = 0;
            for (int k = 0; k < j; k++) {
                sum += l[i][k] * u[k][j];
            }
            if (u[j][j] == 0) {
               
                return 1;
            }
          l[i][j] = (A(i,j) - sum) / u[j][j]; //clen dolni trojuhelnikova matice L
        }
        //nemusi byt az tak potrebne, ale je to jednodussi
       l[i][i] = 1;
        //spocitame cast U - ukoncime radek
        for (int j = i; j <n; j++) {
           float sum = 0;
            for (int k = 0; k < i; k++) {
                sum += l[i][k] * u[k][j];
            }
            u[i][j] = A(i,j)- sum; //clen horni trojuhelnikova matice U

        }
    }
    //Reseni
		// Perform forward substitution for Ly = b.
		for (int i = 0; i < n; i++){
    
			float sum = b[i];
      
			for (int k = 0; k < i; k++){
				sum -= l[i][k]*x[k];
        }
        
			 x[i] = sum;
		}
		
		// Perform backward substitution for Ux = y.
		for (int i = n - 1; i >= 0; i--){
    
			float sum = x[i];
			for (int k = i + 1; k < n; k++){
				sum -= u[i][k]*x[k];
      }
		
    	x[i] = sum/u[i][i];
		}
		
    
     for (int i = 0; i < n; i++) {
      delete[]  l[i] ;
        delete []u[i];
        
    }
    delete []l;
    delete []u;
		
	}
};

#endif

