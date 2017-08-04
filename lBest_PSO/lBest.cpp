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
#include "lBest.h"
#include <omp.h>
#include <gsl/gsl_rng.h>
#include <sys/time.h>

//function prototypes
void setup();
agent updateVelocity(agent agent);
agent updatePosition(agent agent);
double fitness(double pos[N]);
double linearlyDecay(double inertialWeight);
double outsideWall(agent agent);
agent findlBest(agent agents[3]);
double rng(); // Returns a random double between 0 and 1, exclusive; e.g., (0,1)

//Globals
int numAgents = 40;
double* gBest = new double[N];
double* iterationlBest = new double[N];
int numIterations = 3000;


//Velocity and Position Globals
double inertialWeightMax = 0.9;
double inertialWeightMin = 0.4;
double inertialWeight = 0.9;
double c1 = 2.0, c2 = 2.0;

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
    
    
    
    
    agent currentAgent;
    agent previousAgent;
    agent nextAgent;
    agent lBestAgent;
    
    int num = 0;
    
    //initlize solution space
    setup();
    
    //run through each iteration
    for (int i = 0; i < numIterations; i++){
        //  printf("\n--------------Iteration: %d---------------\n\n", i + 1);
        
        
        //for each iteration, run through all agents, evaluate fitness and find best iteration fitness
        for (int j = 0; j < numAgents; j++){
            
            
            //Find current and adjacent agents
            currentAgent = agentVector[j];
            if (j == 0){
                previousAgent = agentVector[numAgents - 1];
                nextAgent = agentVector[j+1];
            }else if (j == numAgents - 1){
                previousAgent = agentVector[j - 1];
                nextAgent = agentVector[0];
            }else {
                previousAgent = agentVector[j - 1];
                nextAgent = agentVector[j + 1];
            }
            
            //if outside wall set fitness = infinity
            currentAgent.fitness = outsideWall(currentAgent);
            previousAgent.fitness = outsideWall(previousAgent);
            nextAgent.fitness = outsideWall(nextAgent);
            
            //if agents inside wall evaluate fitness
            if (currentAgent.fitness != INFINITY  ) {
                //evaluate current and adjacents agents fitness
                currentAgent.fitness = fitness(currentAgent.pos);
                
               // printf("Current Fitness: %f\n", currentAgent.fitness);
                
            }
            if (previousAgent.fitness != INFINITY){
                previousAgent.fitness = fitness(previousAgent.pos);
              //  printf("Previous Fitness: %f\n", previousAgent.fitness);
            }
            if (nextAgent.fitness != INFINITY){
                nextAgent.fitness = fitness(nextAgent.pos);
              //  printf("Next Fitness: %f\n", nextAgent.fitness);
            }
            
            
            //find lbest fitness value for each iteration
            
            agent agents[3];
            agents[0] = previousAgent;
            agents[1] = currentAgent;
            agents[2] = nextAgent;
            lBestAgent = findlBest(agents);
            
            //find best lBest of each iteration
            if (lBestAgent.fitness < fitness(iterationlBest)){
                for (int dim = 0; dim < N; dim++){
                    iterationlBest[dim] = lBestAgent.pos[dim];
                }
            }
            
            
            
            //if current agents position is greater or less than current pBest, set pBest to current agents postion
            if (currentAgent.fitness < fitness(currentAgent.pBest)){
                for (int dim = 0; dim < N; dim++){
                    currentAgent.pBest[dim] = currentAgent.pos[dim];
                }
            }
            
        }
        /*
        printf("Lbest:");
        for (int dim = 0; dim < N; dim++){
            printf(" %f", iterationlBest[dim]);
        }
        printf("\n\n");*/
        num++;

        
        //Replace gBest with iterationBest if it is better
        if (fitness(iterationlBest) < fitness(gBest)){
            for (int dim = 0; dim < N; dim++){
                gBest[dim] = iterationlBest[dim];
                
            }
        }

        
      
        

        
        // printf("\n--------------- Iteration %d -----------------\n\n", num);
        for (int j = 0; j < numAgents; j++){
        /*
            printf("%d Initial Agent Pos:", j);
            
            for (int dim = 0; dim < N; dim++){
                
                printf(" %f ", agentVector[j].pos[dim]);
            }
            printf("Vel:");
            
            for (int dim = 0; dim < N; dim++){
                
                printf(" %f ", agentVector[j].velocity[dim]);
            }
            printf("\n");*/
            
            agentVector[j] = updateVelocity(currentAgent, lBestAgent);
            agentVector[j] = updatePosition(currentAgent);
            
            /*
            printf("%d Updated Agent Pos:", j);
            
            for (int dim = 0; dim < N; dim++){
                
                printf(" %f ", agentVector[j].pos[dim]);
            }
            printf("Vel:");
            
            for (int dim = 0; dim < N; dim++){
                
                printf(" %f ", agentVector[j].velocity[dim]);
            }
            
            printf("\n\n");*/
        }

        
        //linearly decay inertial weight starting from 0.9 to 0.4 over numIterations
        inertialWeight = linearlyDecay(inertialWeight);
        
    }
    
    
    
    
    printf("\n");
    for (int dim = 0; dim < N; dim++){
        
        printf("Global Max %d: %.30f \n", dim, gBest[dim]);
    }
    
    printf("\n\n");
    
    printf("%d\n", num);
    
    printf("\n%f\n", fitness(gBest));
    
    

    return 0;
}

//load agent vector with numAgents agents with random x and velocity
void setup(){
    

    
    //load agent vector with randomized initial population
    for (int i = 0; i < numAgents; i++){
        agent agent;
        
        for (int dim = 0; dim < N; dim++){
            
            agent.pos[dim] = (xMax/2) + (rng()) * (xMax/2);
            
            agent.velocity[dim] = (vxMax/2) + (rng()) * (vxMax/2);
            
            agent.pBest[dim] = agent.pos[dim];
            
        }
        
        agent.fitness = fitness(agent.pos);
        
        agentVector.push_back(agent);
        
        
    }
    
    agent currentAgent;
    agent previousAgent;
    agent nextAgent;
    agent lBestAgent;
    
    double maxFitness = INFINITY;
    double lBestFitness = INFINITY;
    
    //go through initial setup to find first gBest
    for (int j = 0; j < numAgents; j++){
        
        
        //Find current and adjacent agents
        currentAgent = agentVector[j];
        if (j == 0){
            previousAgent = agentVector[numAgents - 1];
            nextAgent = agentVector[j+1];
        }else if (j == numAgents - 1){
            previousAgent = agentVector[j - 1];
            nextAgent = agentVector[0];
        }else {
            previousAgent = agentVector[j - 1];
            nextAgent = agentVector[j + 1];
        }
        
        
        //evaluate current and adjacents agents fitness
        currentAgent.fitness = fitness(currentAgent.pos);
        previousAgent.fitness = fitness(previousAgent.pos);
        nextAgent.fitness = fitness(nextAgent.pos);
        
        
        
        //find lbest fitness value for each iteration
        agent agents[3];
        agents[0] = previousAgent;
        agents[1] = currentAgent;
        agents[2] = nextAgent;
        lBestAgent = findlBest(agents);
        
        //find best lBest of each iteration
        if (lBestAgent.fitness < lBestFitness){
            for (int dim = 0; dim < N; dim++){
                iterationlBest[dim] = lBestAgent.pos[dim];
            }
        }
        
        
        if (agentVector[j].fitness < maxFitness){
            maxFitness = agentVector[j].fitness;
            for (int dim = 0; dim < N; dim++){
                
                gBest[dim] = agentVector[j].pos[dim];
                // iterationlBest[dim] = gBest[dim];
                
            }
            
        }
    }
    
    printf("\nInitial lBest: (%.30f \n, %.30f, \n %.30f)\n", iterationlBest[0], iterationlBest[1], iterationlBest[2]);
    printf("\nInitial gBest: (%.30f \n, %.30f, \n %.30f)\n", gBest[0], gBest[1], gBest[2]);
    
    
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
agent updateVelocity(agent currentAgent, agent lBestAgent){
    
   
    
    for (int dim = 0; dim < N; dim++){
        
       
        
        //calculate Xn velocity
        currentAgent.velocity[dim] = inertialWeight * currentAgent.velocity[dim] + c1 * (rng() * (currentAgent.pBest[dim] - currentAgent.pos[dim])) + c2 * (rng() * (lBestAgent.lBest[dim] - currentAgent.pos[dim]));
        
        /*
         //calculate constricted Xn velocity
         c1 = 2.05;
         c2 = 2.05;
         
         agent.velocity[dim] = 0.72984*(inertialWeight * agent.velocity[dim] + c1 * (((double) rand() / (RAND_MAX)) * (agent.pBest[dim] - agent.pos[dim])) + c2 * (((double) rand() / (RAND_MAX)) * (gBest[dim] - agent.pos[dim])));
         */
        
    }
    
  
    
    //make sure agents velocity is within specified velocity range
    //cap agents velocity to vMax if velocity is over the max range
    for (int dim = 0; dim < N; dim++){
        if (currentAgent.velocity[dim] > vxMax){
            currentAgent.velocity[dim] = vxMax;
        }
        if (currentAgent.velocity[dim] < vxMin){
            currentAgent.velocity[dim] = vxMin;
        }
        
    }
    
     
   
     
    return currentAgent;
    
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

agent findlBest(agent agents[3]){
    
    agent lBestAgent;
    
    lBestAgent.fitness = INFINITY;
    
    for (int i = 0; i < 3; i++){
        
        if (agents[i].fitness < lBestAgent.fitness){
            
            lBestAgent = agents[i];
        }
        
    }
    
    return lBestAgent;
    
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


