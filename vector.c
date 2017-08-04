//
//  vector.c
//  Vectors
//
//  Created by Elliot Eckholm on 7/3/17.
//  Copyright © 2017 Elliot Eckholm. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

void vector_init(vector *v)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->agents = malloc(sizeof(agent) * v->capacity);
}

int vector_total(vector *v)
{
    return v->total;
}

static void vector_resize(vector *v, int capacity)
{
#ifdef DEBUG_ON
    printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif
    
    void **agents = realloc(v->agents, sizeof(agents) * capacity);
    if (agents) {
        v->agents = agents;
        v->capacity = capacity;
    }
}

void vector_add(vector *v, agent *agent)
{
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    v->agents[v->total++] = agent;
}

void vector_set(vector *v, int index, agent *agent)
{
    if (index >= 0 && index < v->total)
        v->agents[index] = agent;
}

void *vector_get(vector *v, int index)
{
    if (index >= 0 && index < v->total)
        return v->agents[index];
    return NULL;
}

void vector_delete(vector *v, int index)
{
    if (index < 0 || index >= v->total)
        return;
    
    v->agents[index] = NULL;
    
    for (int i = index; i < v->total - 1; i++) {
        v->agents[i] = v->agents[i + 1];
        v->agents[i + 1] = NULL;
    }
    
    v->total--;
    
    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

void vector_free(vector *v)
{
    free(v->agents);
}
