
#include "operation.h"

int main(int argc, const char * argv[])
{
   
    
    //generate equations of the first stage
    FILE* f = fopen("../data/equ_files/equations_pre.txt","w+");
    
    
    char state[Five][Five][LANE][equLength];
    UINT64 value[Five][Five];
    bool vars[Five * Five]={1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0};  //1 means this lane is unknown,0 means constants
    UINT64 sum0[Five] = {1,0,0,0,1};  //2 means the sum has variables. The values are the bitwise sum of two CP.
    UINT64 sum1[Five] = {2,2,2,0,1};
    UINT64 sum2[Five] = {2,2,2,2,2};

    for (int i = 0 ; i < Five; i++) {
        for (int j = 0 ; j < Five; j++) {
            value[i][j] = 0;
        }
    }
    
    value[0][1] = UINT64_MAX;
    value[0][3] = UINT64_MAX;
    
    
    //initial
    initState(state, value);
    initVars(state, vars);

    
    //the first round
    
    //write 128 linear equations
    writeSumOf2CPs(f, state, sum0, 0);
    
    theta(state, sum0);
    rho(state);
    pi(state);
    chi(state);
    iota(state, 0);

    // the second round
    
    //write 256 linear equations
    writeSumOf2CPs(f, state, sum0, 1);
    rho(state);
    pi(state);
    chi(state);
    iota(state, 1);

    //the third round
    theta(state, sum2);
    rho(state);
    pi(state);
    
    //write 64 equations of guessing values of 32 pairs
    writeGuessEquations(f, state, 600);
    
    chi(state);
    iota(state, 2);

    //129 constraints
    for (int z = 0; z < LANE; z++) {
        int p34 = 0;

        while (state[3][4][z][p34] != ':') {
            p34++;
        }
        state[3][4][z][p34++] = '+';
        int p33 = 0;
        while (state[3][3][z][p33] != ':') {
            state[3][4][z][p34++] = state[3][3][z][p33++];

        }
        state[3][4][z][p34++] = '+';
        state[3][4][z][p34++] = '1';
        state[3][4][z][p34++] = ':';
        int p44 = 0;
        
        while (state[4][4][z][p44] != ':') {
            p44++;
        }
        state[4][4][z][p44++] = '+';
        int p43 = 0;
        while (state[4][3][z][p43] != ':') {
            state[4][4][z][p44++] = state[4][3][z][p43++];
            
        }
        state[4][4][z][p44++] = '+';
        state[4][4][z][p44++] = '1';
        state[4][4][z][p44++] = ':';

    }

    int p24 = 0;
    while(state[2][4][63][p24] != ':'){
        p24++;
    }
    state[2][4][63][p24++] = '+';
    int p23 = 0;
    while(state[2][3][63][p23] != ':'){
        state[2][4][63][p24++] = state[2][3][63][p23++];
    }
    state[2][4][63][p24++] = ':';



    
    //write 129 quadratic equations
    writeEquations(f, state);
    
    return 0;
}

