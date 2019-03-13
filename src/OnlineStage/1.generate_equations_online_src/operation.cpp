

#include "operation.h"


void printState(char state[Five][Five][LANE][equLength]){
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



void theta_b2r1(char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five]){
    // ci+ci'= 0 : column 3 58 60, column 4 36 37 40 48 54

    
    
    
    for (int x = 0; x < Five; x++) {

        for (int z = 0 ; z < 64; z++) {
            if (cpkernal[x]&((UINT64)1 << z)) {
                for (int y = 0; y < Five; y++) {
                    
                    int p = findLastPos(state[x][y][z]);
                     if (p>0) {
                        state[x][y][z][p++] = '+';
                        
                    }
                    state[x][y][z][p++] = '1';
                    state[x][y][z][p++] = ':';
                    if (state[x][y][z][0] == '1' && state[x][y][z][1] == '+' && state[x][y][z][2] == '1') {
                        state[x][y][z][0] = ':';
                    }
                }
            }else{
                continue;

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

/**
 * read meaningful char from file.
 */
static inline void meaningchar(FILE *f, char *p_c) { //r
    do {
        fscanf(f, "%c", p_c);
        //printf("%d ",p_c);
    } while ((*p_c) != 'x' && !((*p_c) >= '0' && (*p_c) <= '9') && (*p_c) != '(' && (*p_c) != ')' && (*p_c) != '.' && (*p_c) != ';' && (*p_c) != ',' && (*p_c) != '+');
    //printf("\n");
}

/**
 * read meaningful char from file.
 */
static inline void meaningchar_p(FILE *f, char *p_c) { //r
    do {
        fscanf(f, "%c", p_c);
    } while ((*p_c) != 'x' && !((*p_c) >= '0' && (*p_c) <= '9') && (*p_c) != '(' && (*p_c) != ')' && (*p_c) != '.' && (*p_c) != ';' && (*p_c) != ',' && (*p_c) != '+' && (*p_c) != 'a' && (*p_c) != 'b' && (*p_c) != '*');
}

void printLinearParamEquations(FILE *f_out, FILE *f,char state[Five][Five][LANE][equLength], int start_pos){
    char c;
    meaningchar_p(f, &c);

    while (c != '.') {
        if (c == 'x') {
            int d = 0;
            meaningchar_p(f, &c);
            while (c <= '9' && c >= '0') {
                d = d*10+c - '0';
                meaningchar_p(f, &c);
            }
            
            d = d - 700;
            int z = d % LANE;
            int x = d / LANE;
            int n = 0;
            while (state[x][0][z][n] != ':') {
                fprintf(f_out, "%c",state[x][0][z][n++]);
            }
        }
        
        if (c == '+' || c == '1' || c == ',' || c == '(' || c == ')' || c =='*') {
            fprintf(f_out," %c ",c);
            if (c == ',') {
                fprintf(f_out, "\n");
            }
            
        }
        if(c == 'a'){
            fprintf(f_out, "x%d", start_pos);
        }
        
        //if(c == 'b'){
            //fprintf(f_out, "x%d", start_pos+1);
        //}
        meaningchar_p(f, &c);
        //printf("%c ",c);
    }
    
    fprintf(f_out, ",\n");
}



void printLinearRelationship(FILE * f_out, FILE *f,char state[Five][Five][LANE][equLength]){
    char c;
    fseek(f, 0L, SEEK_SET);
    meaningchar(f, &c);
    

    
    
    
    while (c != '.') {

        if (c == 'x') {
            int d = 0;
            meaningchar(f, &c);
            while (c <= '9' && c >= '0') {

                d = d*10+c - '0';
                meaningchar(f, &c);
                //printf("%c\n",c);
            }
            
            d = d - 700;
            int z = d % LANE;
            int x = d / LANE;
            int n = 0;
            
            int tt = 0;
            while (state[x][0][z][n] != ':') {
                fprintf(f_out, "%c", state[x][0][z][n++]);
                //printf("%d\n",tt++);
            }
           
        }
        //printf("4\n");
        if (c == '+' || c == '1' || c == ',') {
            fprintf(f_out, " %c ",c);
            if (c == ',') {
                fprintf(f_out, "\n");
            }
        }
        
        
        //printf("5\n");
        meaningchar(f, &c);
        //printf("6\n");
       // printf("%c\n",c);
    }
    //printf("out\n");
    fprintf(f_out, ".");
    
}


int writeGuessedVariables(FILE *f, char state[Five][Five][LANE][equLength], int start_param){
    int num = 0;
    for(int y = 0; y < 5; y++){
        for(int x = 0; x < 5; x++){
            for(int z = 0; z < 64; z++){
                if(x == 1 || x==3 || x==4 ){
                    if(state[x][y][z][0] != ':' && state[x][y][z][1] != ':'){
                        
                        int p = 0;
                        while (state[x][y][z][p] != ':' && state[x][y][z][p] != '+') {
                            fprintf(f, "%c",state[x][y][z][p++]);
                        }
                        fprintf(f, " + x%d,\n", start_param + num);
                        num++;
                    }
                }
            }
        }
    }
    return num;
}


int writeSumOf2CPs(FILE *f, char state[Five][Five][LANE][equLength],  int start_param){

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
    
    int guessed_num = 0;
    
    // print linear constraints
    fprintf(f, "\n");
    for (int z = 1; z < LANE; z++) {
        int p = findLastPos(sum[0][z]);
        for (int pp = 0; pp < p; pp++) {
            fprintf(f, "%c",sum[0][z][pp]);
        }
        fprintf(f, " + 1,\n");
        /*
        if(guessed_num < 31){
            fprintf(f, " + x%d,\n", start_param + guessed_num);
            guessed_num ++;
        }else{
            fprintf(f, ",\n");
        }
        */
    }
    
    fprintf(f, "\n");
    for (int z = 0; z < LANE; z++) {
        int p = findLastPos(sum[2][z]);
        for (int pp = 0; pp < p; pp++) {
            fprintf(f, "%c",sum[2][z][pp]);
        }
        if(guessed_num < 31){
            fprintf(f, " + x%d,\n", start_param + guessed_num);
            guessed_num ++;
        }else{
            fprintf(f, ",\n");
        }
    }
    
    return guessed_num;

}


void writeSumOf5CPs(FILE *f, char state[Five][Five][LANE][equLength], UINT64 cpkernal[Five]){
    
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
    
    
    
    
    //print linear constaints
    //printf("sum:\n");
    for (int x = 0; x < Five; x++) {
        for (int z = 0 ; z < 64; z++) {
           /*
            int sp = findLastPos(sum[x][z]);
            for (int pp  = 0 ; pp < sp; pp++) {
                printf("%c",sum[x][z][pp]);
                    
            }
            printf("  +  ");
            sp = findLastPos(sum[MODFive(x+2)][z]);
            for (int pp  = 0 ; pp < sp; pp++) {
                printf("%c",sum[MODFive(x+2)][z][pp]);
                    
            }
            printf(",\n");
            */
            for (int y = 0 ; y < Five; y++) {
                int p = findLastPos(state[x][y][z]);
                if (p > 0 && y > 0) {
                    fprintf(f, "+");
                }
                for (int pp  = 0 ; pp < p; pp++) {
                    fprintf(f, "%c",state[x][y][z][pp]);
                    
                }
                p = findLastPos(state[MODFive(x+2)][y][MODLane(z-1)]);
                if(p > 0){
                    fprintf(f, "+");
                }
                for (int pp  = 0 ; pp < p; pp++) {
                    fprintf(f, "%c",state[MODFive(x+2)][y][MODLane(z-1)][pp]);
                }
                
                
            }
            
            if (cpkernal[MODFive(x+1)] & ((UINT64)1 << z)) {
                fprintf(f, " +1,\n");
            }else{
                fprintf(f, ",\n");
            }
            
        }
    }
}
