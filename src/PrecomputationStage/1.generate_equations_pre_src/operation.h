

#ifndef __GenKeccakEquations__operation__
#define __GenKeccakEquations__operation__

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include "config.h"
void initState(char state[Five][Five][LANE][equLength], UINT64 value[Five][Five]);
void initVars(char state[Five][Five][LANE][equLength], bool vars[Five * Five]);

void theta_with_output(char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five]);
void theta(char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five]);
void rho(char state[Five][Five][LANE][equLength]);
void pi(char state[Five][Five][LANE][equLength]);
void chi(char state[Five][Five][LANE][equLength]);
void iota(char state[Five][Five][LANE][equLength],int round);


void eliminateOne(char state[equLength]);


void printAllState(char state[Five][Five][64][equLength]);
void writeEquations(FILE* f, char state[Five][Five][LANE][equLength]);
void writeSumOf2CPs(FILE* f, char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five], int round);
void writeGuessEquations(FILE *f, char state[Five][Five][LANE][equLength], int start_idx);
#endif /* defined(__GenKeccakEquations__operation__) */
