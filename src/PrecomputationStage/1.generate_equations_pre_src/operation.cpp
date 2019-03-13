

#include "operation.h"


//print equations of all bits to screen
void printAllState(char state[Five][Five][LANE][equLength]){
    for (int y = 0; y < Five; y++) {
        for (int x = 0; x < Five; x++) {
            printf("---------- state( %d , %d ) ----------\n",x,y);
            for (int z =0 ; z < LANE; z ++) {
//                printf("%d : ",z);
                int n = 0;
                while (state[x][y][z][n] != ':') {
                    printf("%c",state[x][y][z][n++]);
                }
                printf(",\n");
            }
        }
    }
}

//write quadratic equations to the file
void writeEquations(FILE* f, char state[Five][Five][LANE][equLength]){

    int n = 0;

        
    for (int z =0 ; z < LANE; z ++) {
        n = 0;
        while (state[3][4][z][n] != ':') {
            fprintf(f,"%c",state[3][4][z][n++]);
        }
        fprintf(f,",\n");
    }
    
    for (int z =0 ; z < LANE; z ++) {
        n = 0;
        while (state[4][4][z][n] != ':') {
            fprintf(f,"%c",state[4][4][z][n++]);
        }
        fprintf(f,",\n");
    }
    
    n = 0;
    while (state[2][4][63][n] != ':') {
        fprintf(f,"%c",state[2][4][63][n++]);
    }
    fprintf(f,".\n");
    
}


// initial state
void initState(char state[Five][Five][LANE][equLength], UINT64 value[Five][Five]){
    
    for (int y = 0; y < Five; y++) {
        for (int x = 0; x < Five; x++) {
            for (int z = 0; z < LANE; z++) {
                if(value[y][x] & ((UINT64)1 << z)){
                    state[x][y][z][0] = '1';
                    state[x][y][z][1] = ':';
                }else{
                    state[x][y][z][0] = ':';
                }
            }
        }
    }
    
    
}

//initial variables
void initVars(char state[Five][Five][LANE][equLength], bool vars[Five * Five]){
    
    short nvar = 0;
    for (int y = 0; y < Five; y++) {
        for (int x = 0 ; x < Five; x++) {
            if (vars[x+ Five * y]) {
                for (int z = 0; z < LANE; z++) {
                    short len = 0;
                    state[x][y][z][len++] = 'x';
                    if (len > 1 || nvar/1000) {
                        state[x][y][z][len++] = nvar/1000 + 48;
                    }
                    if (len > 1 || (nvar%1000)/100) {
                        state[x][y][z][len++] = (nvar%1000)/100 + 48;
                    }
                    if (len > 1 || (nvar%100)/10) {
                        state[x][y][z][len++] = (nvar%100)/10 + 48;
                    }
                    if (len > 1 || (nvar%10)) {
                        state[x][y][z][len++] = (nvar%10) + 48;
                    }
                    if (nvar == 0) {
                        state[x][y][z][len++] = 48;
                    }
                    state[x][y][z][len] = ':';
                    nvar++;
                }
            }
        }
    }
}


int findLastPos(char* state){
    int pos = 0;
    while (state[pos] != ':') {
        pos++;
    }
    return pos;
}

bool findPlus(char* state){
    int pos = 0;
    while (state[pos] != ':') {
        if(state[pos++] == '+'){
            return true;
        }
    }
    return false;
}


void eliminateOne(char state[equLength]){
    
    int oneNum = 0;
    char temp[equLength];
    int p = findLastPos(state);
    int l = 0;
    for (int pp = 0; pp < p ; pp ++) {
        if(state[pp] == '+' && state[pp + 1] == '1'){
            oneNum ++;
            pp ++;
            continue;
        }
        temp[l++] = state[pp];
    }
    if(oneNum % 2){
        temp[l++] = '+';
        temp[l++] = '1';
    }
    temp[l++] = ':';

    memcpy(state, temp, sizeof(temp));
    
}

void eliminateOnePair(char state[equLength]){
    
    int oneNum = 0;
    char temp[equLength];
    int p = findLastPos(state);
    int l = 0;
    for (int pp = 0; pp < p ; pp ++) {
        if(state[pp] == '+' && state[pp + 1] == '1'&&state[pp+2] == '+' && state[pp + 3] == '1'){
            
            pp = pp + 3;
            continue;
        }
        temp[l++] = state[pp];
    }
    temp[l++] = ':';
    
    memcpy(state, temp, sizeof(temp));
    
}

//write linear constraints
void writeSumOf2CPs(FILE* f, char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five], int round){
    char sum[Five][LANE][equLength];
    for (int x = 0; x < Five; x++) {

        for (int z = 0; z < LANE; z++) {
            int psum = 0;
            int lastsum = 0;
            while (state[x][0][z][lastsum] != ':') {
                sum[x][z][psum++] = state[x][0][z][lastsum++];
            }
            
            for (int y = 1; y < Five; y++) {
                if(psum>0 && findLastPos(state[x][y][z]) > 0){
                    sum[x][z][psum++] = '+';
                }
                lastsum = 0;
                while (state[x][y][z][lastsum] != ':') {
                    sum[x][z][psum++] = state[x][y][z][lastsum++];
                    
                }
            }
            
            if(sum[x][z][psum-1] == '+'){
                sum[x][z][psum - 1] = ':';
            }else{
                
                sum[x][z][psum] = ':';
            }
            
           
        }
    }
    
    if(round == 0){

    for (int x = 3; x < Five; x++) {
//         if (cpkernal[x] == 0 || cpkernal[x] ==1) {
             fprintf(f, "\n");
             for (int z = 0; z < LANE; z++) {
                     int p1 = findLastPos(sum[MODFive(x-1)][z]);
                     for (int pp = 0; pp < p1; pp++) {
                         fprintf(f, "%c",sum[MODFive(x-1)][z][pp]);
                     }
                     
                     int p2 = findLastPos(sum[MODFive(x+1)][MODLane(z-1)]);
                     if (p1 > 0 && p2 > 0) {
                         fprintf(f, "+");

                     }

                     for (int pp = 0; pp < p2; pp++) {
                         fprintf(f, "%c",sum[MODFive(x+1)][MODLane(z-1)][pp]);
                     }
                     if (cpkernal[x] == 1) {
                         fprintf(f, "+1");
                     }
                     fprintf(f, ",\n");
                 
             }
//         }
         
     }
    
    
    }
    
    if(round == 1){
        for (int x = 1; x < Five ; x++) {

             fprintf(f, "\n");
             for (int z = 0; z < LANE; z++) {
                int p1 = findLastPos(sum[MODFive(x-1)][z]);
                for (int pp = 0; pp < p1; pp++) {
                   fprintf(f, "%c",sum[MODFive(x-1)][z][pp]);
                }
                 
                fprintf(f,",\n");
                 
             }

         
        }
    
    }
}



void theta(char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five]){
    char sum[Five][LANE][equLength];
    for (int x = 0; x < Five; x++) {

        for (int z = 0; z < LANE; z++) {
            int psum = 0;
            int lastsum = 0;
            while (state[x][0][z][lastsum] != ':') {
                sum[x][z][psum++] = state[x][0][z][lastsum++];
            }
            
            for (int y = 1; y < Five; y++) {
                if(psum>0 && findLastPos(state[x][y][z]) > 0){
                    sum[x][z][psum++] = '+';
                }
                lastsum = 0;
                while (state[x][y][z][lastsum] != ':') {
                    sum[x][z][psum++] = state[x][y][z][lastsum++];
                    
                }
            }
            
            if(sum[x][z][psum-1] == '+'){
                sum[x][z][psum - 1] = ':';
            }else{
                
                sum[x][z][psum] = ':';
            }
            
           
        }
    }
    
    for (int x = 0; x < Five; x++) {
        if (cpkernal[x] == 0) {
            continue;
        }else if(cpkernal[x] == 1){
            for (int y = 0; y < Five; y++) {
                for (int z = 0; z < LANE; z++) {
                    int p = findLastPos(state[x][y][z]);
                    if (p>0) {
                        state[x][y][z][p++] = '+';

                    }
                    state[x][y][z][p++] = '1';
                    state[x][y][z][p++] = ':';
                }
            }
        }else{
            
            
            
            for (int y = 0; y < Five; y++) {
                for (int z = 0; z < LANE; z++) {
                    int p = findLastPos(state[x][y][z]);
//                    int p = 0;
                    int ps1 = findLastPos(sum[MODFive(x-1)][z]);
                    if (p>0 && ps1 >0  ) {
                        state[x][y][z][p++] = '+';
                        
                    }
                    int last = 0;
                    while (sum[MODFive(x-1)][z][last] != ':') {
                        state[x][y][z][p++] = sum[MODFive(x-1)][z][last++];
                    }
                    int ps2 = findLastPos(sum[MODFive(x+1)][MODLane(z-1)]);
                    if(p>0 && ps2 >0){
                        state[x][y][z][p++] = '+';
                    
                    }
                    last = 0;
                    while (sum[MODFive(x+1)][MODLane(z-1)][last] != ':') {
                        state[x][y][z][p++] = sum[MODFive(x+1)][MODLane(z-1)][last++];
                    }
                    
                    if (state[x][y][z][p-1]=='+') {
                        state[x][y][z][p-1] = ':';
                    }else{
                        state[x][y][z][p] = ':';

                    }
                    
                }
            }
        }
    }
    
}


void rho(char state[Five][Five][LANE][equLength]){
    for (int y = 0; y < Five; y++) {
        for (int x = 0; x < Five; x++) {
            int rhoValue = KeccakRhoOffsets[index(x, y)];
            char temp[LANE][equLength];
            memcpy(temp, state[x][y], sizeof(char)*LANE*equLength);
            for (int z = 0; z < LANE; z++) {
                memcpy(state[x][y][MODLane(z+rhoValue)], temp[z], sizeof(char)*equLength);
            }
        }
    }
}

void pi(char state[Five][Five][LANE][equLength]){
    
    
    for (int z = 0; z < LANE; z++) {
        char temp[Five][Five][equLength];
        for (int x = 0; x < Five; x++) {
            for (int y = 0; y < Five; y++) {
                int p = findLastPos(state[x][y][z]);
                for (int pp = 0; pp < p+1; pp ++) {
                    temp[x][y][pp] = state[x][y][z][pp];
                }
            }
        }
        
        
        for (int x = 0; x < Five ; x++) {
            for (int y= 0; y < Five; y++) {
                memcpy(state[MODFive(0*x+1*y)][MODFive(2*x+3*y)][z],temp[x][y], sizeof(temp[x][y]));
                
            }
        }
        
        
    }
    
}


void chi(char state[Five][Five][LANE][equLength]){
 
    
    
    for (int y = 0; y < Five; y++) {
        for (int z = 0; z < LANE; z++) {
            char temp[Five][equLength];
            for (int x = 0; x < Five; x++) {
                int p = findLastPos(state[x][y][z]);
                for (int pp = 0; pp < p+ 1; pp ++) {
                    temp[x][pp] = state[x][y][z][pp];
                }
            }
            
            int pos[Five];
            for (int x = 0; x < Five; x++) {
                pos[x] = findLastPos(temp[x]);
            }
            
            for (int x = 0; x < Five; x++) {
                int p = 0;
                for (p = 0; p < pos[x]; p++) {
                    state[x][y][z][p] = temp[x][p];
                }
                
                if(pos[MODFive(x+1)] == 1 || pos[MODFive(x+2)] == 0){
                    state[x][y][z][p] = ':';
                }else{
                    if (pos[MODFive(x+1)] > 1 && pos[MODFive(x+2)] > 1 ) {
                        if (p > 0) {
                            state[x][y][z][p++] = '+';

                        }
                        state[x][y][z][p++] = '(';
                        
                        for (int i = 0; i < pos[MODFive(x+1)]; i++) {
                                state[x][y][z][p++] = temp[MODFive(x+1)][i];
                        }
                        
                        if (p > 0) {

                        
                            state[x][y][z][p++] = '+';

                        }
                        state[x][y][z][p++] = '1';
                        
                        state[x][y][z][p++] = ')';
                            
                        
                        if(p > 0){
                            state[x][y][z][p++] = '*';
                        }
                        
                        if(findPlus(state[MODFive(x+2)][y][z])){
                            state[x][y][z][p++] = '(';
                        }
                        for (int i = 0; i < pos[MODFive(x+2)]; i++) {
                            state[x][y][z][p++] = temp[MODFive(x+2)][i];
                        }
                        
                        if(findPlus(state[MODFive(x+2)][y][z])){
                            
                            state[x][y][z][p++] = ')';
                            
                        }
                    }else if(pos[MODFive(x+1)] > 1 && pos[MODFive(x+2)] == 1 ){
                        if(p > 0){
                            state[x][y][z][p++] = '+';
                        }
                        for (int i = 0; i < pos[MODFive(x+1)]; i++) {
                            state[x][y][z][p++] = temp[MODFive(x+1)][i];
                        }
                        if (p > 0) {

                            state[x][y][z][p++] = '+';

                        }
                        state[x][y][z][p++] = '1';

                        
                    }else if(pos[MODFive(x+1)]== 0 && pos[MODFive(x+2)] > 1 ){
                        if(p > 0){
                            state[x][y][z][p++] = '+';
                        }
                        for (int i = 0; i < pos[MODFive(x+2)]; i++) {
                            state[x][y][z][p++] = temp[MODFive(x+2)][i];
                        }
                        
                    }else{
                        if(p > 0){

                            state[x][y][z][p++] = '+';
                        }
                        state[x][y][z][p++] = '1';

                    }
                    
                }
                state[x][y][z][p++] = ':';

                
            }

        }
    }
    
    for (int x= 0; x < Five; x ++) {
        for (int y = 0; y < Five; y ++) {
            for (int z = 0; z < LANE; z++) {
                eliminateOnePair(state[x][y][z]);
            }
        }
    }
    
}


void iota(char state[Five][Five][LANE][equLength],int round){
    
    for (int z = 0; z < LANE; z++) {
        if(KeccakRoundConstants[round] & ((UINT64)1) << z ){
            int p = findLastPos(state[0][0][z]);
            state[0][0][z][p++] = '+';
            state[0][0][z][p++] = '1';
            state[0][0][z][p++] = ':';


        }
    }
    
}


void writeGuessEquations(FILE *f, char state[Five][Five][LANE][equLength], int start_idx){
    int idx = start_idx;
    for(int z = 0 ; z < 32; z++){
        int p3 = findLastPos(state[0][3][z]);
        for (int pp = 0; pp < p3; pp++) {
            fprintf(f,"%c", state[0][3][z][pp]);
        }
        fprintf(f, " + x%d,\n", idx);
        idx ++;
    }
    

    for(int z = 0 ; z < 32; z++){
        int p4 = findLastPos(state[0][4][z]);
        for (int pp = 0; pp < p4; pp++) {
            fprintf(f, "%c", state[0][4][z][pp]);
        }
        
        fprintf(f, " + x%d,\n", idx);
        
        idx++;
        
    }
    
      
    
    
    
    
    
    
}
