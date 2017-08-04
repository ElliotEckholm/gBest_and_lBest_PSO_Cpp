//
//  main.cpp
//  firstPSO
//
//  Created by Elliot Eckholm on 6/20/17.
//  Copyright © 2017 Elliot Eckholm. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <time.h>
#include "main.h"
#include <gsl/gsl_rng.h>
#include <sys/time.h>



//function prototypes
void setup();
agent updateVelocity(agent agent);
agent updatePosition(agent agent);
double fitness(double pos[N]);
double linearlyDecay(double inertialWeight);
double outsideWall(agent agent);
double rng();

//Globals
int numAgents = 40;
double* gBest = new double[N];
double* iterationBest = new double[N];
int numIterations = 3000;


//Velocity and Position Globals
double inertialWeightMax = 0.9;
double inertialWeightMin = 0.4;
double inertialWeight = 0.9;
double c1 = 2.0, c2 = 2.0;
double timeStep = .1;

//Bounds
int xMax = 600, xMin = -xMax;
int yMax = 600, yMin = -yMax;
int zMax = 600, zMin = -zMax;

double range = 0.2;

int vxMax = range*(xMax - xMin), vxMin = -range*(xMax - xMin);
int vyMax = range*(xMax - xMin), vyMin = -range*(xMax - xMin);
int vzMax = range*(xMax - xMin), vzMin = -range*(xMax - xMin);

//using
using namespace std;

//vector where the agents are stored
vector<agent> agentVector;


int main(int argc, const char * argv[]) {
    
    ofstream myfile;
    myfile.open ("Data.txt");
    
    int num = 0;
    //initlize solution space
    setup();
    
    //run through each iteration
    for (int i = 0; i < numIterations; i++){
      //  printf("\n--------------Iteration: %d---------------\n\n", i + 1);
        
        
        //for each iteration, run through all agents, evaluate fitness and find best iteration fitness
        for (int j = 0; j < numAgents; j++){
            
            //if outside wall set fitness = infinity
            agentVector[j].fitness = outsideWall(agentVector[j]);
            
           //if inside wall evaluate fitness
            if (agentVector[j].fitness != INFINITY) {
                
                //evaluate current agents fitness
                agentVector[j].fitness = fitness(agentVector[j].pos);
               // printf("%.10f\n",agentVector[j].fitness);
               // num++;
               
            }
            
            //find best fitness value for each iteration
            if (agentVector[j].fitness < fitness(iterationBest)){
                for (int dim = 0; dim < N; dim++){
                     iterationBest[dim] = agentVector[j].pos[dim];
                    
                }
            }
           
            //if current agents position is greater or less than current pBest, set pBest to current agents postion
            if (agentVector[j].fitness < fitness(agentVector[j].pBest)){
                for (int dim = 0; dim < N; dim++){
                    agentVector[j].pBest[dim] = agentVector[j].pos[dim];
                }
            }
            
        }
        
    
        //Replace gBest with iterationBest if it is better
        if (fitness(iterationBest) < fitness(gBest)){
            for (int dim = 0; dim < N; dim++){
                gBest[dim] = iterationBest[dim];
                    
            }
        }
        
        num++;
        
       // myfile << fitness(gBest) << "\n" ;
        myfile << num << "\n" ;
        
      //  printf("fitness of iterationBest = %f\n", fitness(iterationBest));
       // printf("iteration fitness of gBest = %f\n", fitness(gBest));
        
        for (int j = 0; j < numAgents; j++){
        
            agentVector[j] = updateVelocity(agentVector[j]);
            agentVector[j] = updatePosition(agentVector[j]);
        
        }
        
        //linearly decay inertial weight starting from 0.9 to 0.4 over numIterations
        inertialWeight = linearlyDecay(inertialWeight);

    }
    
        
    
    
    printf("\n");
    for (int dim = 0; dim < N; dim++){
    
       printf("Global Max %d: %.30f \n", dim, gBest[dim]);
    }
    
    printf("iteration fitness of gBest = %f\n", fitness(gBest));
   // printf("\n %d",num);
   // printf("\n %f", sumFitness / (num));
    printf("\n");
    printf("%d \n", num);
    
    myfile.close();
    
    return 0;
}

//load agent vector with numAgents agents with random x and velocity
void setup(){
    
    
    agentVector.resize(numAgents);
    
   //srand((unsigned)time(0));
    
    //load agent vector with randomized initial population
    for (int i = 0; i < numAgents; i++){
        
        
        for (int dim = 0; dim < N; dim++){
        
            agentVector[i].pos[dim] = (xMax/2) + (rng()) * (xMax/2);
            
            agentVector[i].velocity[dim] = (vxMax/2) + (rng()) * (vxMax/2);
        
            agentVector[i].pBest[dim] = agentVector[i].pos[dim];
            
        }
        
        agentVector[i].fitness = fitness(agentVector[i].pos);
        
    

    }
    
    double maxFitness = INFINITY;
    
    //go through initial setup to find first gBest
    for (int j = 0; j < agentVector.size(); j++){
        
        if (agentVector[j].fitness < maxFitness){
            maxFitness = agentVector[j].fitness;
            for (int dim = 0; dim < N; dim++){
                
                gBest[dim] = agentVector[j].pos[dim];
                iterationBest[dim] = gBest[dim];

            }

        }
    }
    
  //  printf("\nInitial gBest: (%.30f \n, %.30f, \n %.30f)\n", gBest[0], gBest[1], gBest[2]);
    
    
}

//N dimensional fitness function
double fitness(double pos[N]){
    
     double fitnessValue = 0;
    
    
    //Griewank function
    double sum = 0, mult = 1;
    
    for (int dim = 0; dim < N; dim++){
        
        sum += pow(pos[dim], 2);
        
        mult *= ( cos( pos[dim] / (sqrt(dim + 1)) ) );
    }
    
    fitnessValue = (sum / 4000) - (mult) + 1;
    
    
   /*
    //Rastigrin function
    double sum = 0;
    for (int dim = 0; dim < N; dim++){
        sum += (pow(pos[dim],2) - (10 * cos(2*M_PI*pos[dim])) + 10);
    }
    fitnessValue = sum;
    */
    
    /*
    //Rosenbrock function
    double sum = 0;
    for (int dim = 0; dim < N - 1; dim++){
        
        
            sum += (100 * pow((pos[dim+1] - (pow((pos[dim]),2))),2) + pow((pos[dim] - 1),2));
       
        
    }
    fitnessValue = sum;
    

   */
    
    return fitnessValue;
}

//Update veloctiy using velocity function given in Jacob Robinson and Yahya Rahmat-Samii 2004 paper
agent updateVelocity(agent agent){
    
    for (int dim = 0; dim < N; dim++){
        
        //calculate Xn velocity
        agent.velocity[dim] = inertialWeight * agent.velocity[dim] + c1 * (rng() * (agent.pBest[dim] - agent.pos[dim])) + c2 * ((rng()) * (gBest[dim] - agent.pos[dim]));
        
        /*
        //calculate constricted Xn velocity
        c1 = 2.05;
        c2 = 2.05;
        
         agent.velocity[dim] = 0.72984*(inertialWeight * agent.velocity[dim] + c1 * (((double) rand() / (RAND_MAX)) * (agent.pBest[dim] - agent.pos[dim])) + c2 * (((double) rand() / (RAND_MAX)) * (gBest[dim] - agent.pos[dim])));
        */
        
    }
    
    for (int dim = 0; dim < N; dim++){
        if (agent.velocity[dim] > vxMax){
            agent.velocity[dim] = vxMax;
        }
        if (agent.velocity[dim] < vxMin){
            agent.velocity[dim] = vxMin;
        }
    
    }
    
    
    //make sure agents velocity is within specified velocity range
    //cap agents velocity to vMax if velocity is over the max range
  
    
    return agent;
    
}

//Update position using function given in Jacob Robinson and Yahya Rahmat-Samii 2004 paper
agent updatePosition(agent agent){
    
    for (int dim = 0; dim < N; dim++){
        
        agent.pos[dim] = agent.pos[dim] + agent.velocity[dim];
       
        
    }

    return agent;
}

//decrease intertial weight by calculated increment
double linearlyDecay(double inertialWeight){
    
    double increment = (inertialWeightMax - inertialWeightMin) / numIterations;
    
    return inertialWeight - increment;
}


double outsideWall(agent agent){
    
    
    for (int dim = 0; dim < N; dim++){
        
        if (agent.pos[dim] < xMin){
            agent.fitness = INFINITY;
        }
        if (agent.pos[dim] > xMax){
            agent.fitness = INFINITY;
        }
        
    }
    
    return agent.fitness;
    
}

// Returns a random double between 0 and 1, exclusive; e.g., (0,1)
double rng(){
    
    
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();
    struct timeval tv; // Seed generation based on time
    gettimeofday(&tv,0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
    T = gsl_rng_default; // Generator setup
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, mySeed);
    double u = gsl_rng_uniform(r); // Generate it!
    gsl_rng_free (r);
    return u;
    
}

