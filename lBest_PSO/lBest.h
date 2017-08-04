//
//  main.h
//  lBestPSO
//
//  Created by Elliot Eckholm on 6/26/17.
//  Copyright © 2017 Elliot Eckholm. All rights reserved.
//

#ifndef lBest_h
#define lBest_h


#endif /* lBest_h */

//set number of dimensions
int N = 30;

typedef struct {
    double* pos = new double[N]; // (x1,x2,x3,...xN)
    double* velocity = new double[N]; // (v1,v2,v3,...vN)
    double* pBest = new double[N]; // (x1,x2,x3,...xN)
    double fitness;
    double* lBest = new double[N]; // (x1,x2,x3,...xN)
    
} agent;


typedef struct {
    double inertialWeight;
    double inertialWeightMin;
    double inertialWeightMax;
    double range;
    double c1;
    double c2;
    int xMax;
    int xMin;
    double vxMin;
    double vxMax;
    int numAgents;
    int numIterations;
}psoParamStruct;


//typedef double (*fitness)(int nDim, double p[]); //fitness function

//function prototypes
void setup();
agent updateVelocity(agent currentAgent, agent lBestAgent);
agent updatePosition(agent agent);
double fitness(double pos[N]);
double linearlyDecay(double inertialWeight);
double outsideWall(agent agent);
agent findlBest(agent agents[3]);
double rng(); // Returns a random double between 0 and 1, exclusive; e.g., (0,1)




//function prototypes
void pso_set_griewank_settings(psoParamStruct* psoParams);
void pso_set_rastrigin_settings(psoParamStruct* psoParams);
void pso_set_rosenbrock_settings(psoParamStruct* psoParams);
