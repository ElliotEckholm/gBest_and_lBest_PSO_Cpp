//
//  main.h
//  3DPSO
//
//  Created by Elliot Eckholm on 6/20/17.
//  Copyright © 2017 Elliot Eckholm. All rights reserved.
//

#ifndef main_h
#define main_h


#endif /* main_h */

//set number of dimensions
int N = 30;

typedef struct {
    double* pos = new double[N]; // (x1,x2,x3,...xN)
    double* velocity = new double[N]; // (v1,v2,v3,...vN)
    double* pBest = new double[N]; // (x1,x2,x3,...xN)
    double fitness;
    double* lBest = new double[N]; // (x1,x2,x3,...xN)
    
} agent;

