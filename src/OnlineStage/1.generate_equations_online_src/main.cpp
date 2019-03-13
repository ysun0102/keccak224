


#include "operation.h"

int main(int argc, const char * argv[])
{
   
   	printf("1.read file\n");
   	FILE *f = fopen("../../PrecomputationStage/data/state_files/output_pre_file.txt", "r+");
   	
   	char line[100];

   	for(int i = 0; i < 13; i++){
   		fgets(line, 100, f);
   		//printf("%d. %s\n", i, line);   	
   	}
   	
   	UINT64 value[25] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 };
   	
   	
   	//printf("output state of precomputation stage:\n");
   	for(int i = 0; i < 5; i++){
   		char hex_s = 0;
   		UINT64 tmp_val = 0;
   		for(int j = 0; j < 17; j++){

            if(line[i*17 + j] <= '9' && line[i*17 + j] >= '0'){
                hex_s = line[i*17 + j] - '0';
            }else if(line[i*17 + j] <= 'z' && line[i*17 + j] >= 'a'){
                hex_s = line[i*17 + j] - 'a' + 10;
            }else if(line[i*17 + j] <= 'Z' && line[i*17 + j] >= 'A'){
                hex_s = line[i*17 + j] - 'A' + 10;
            }
            
            if(j < 16){
            	//printf("%c", line[i*17 + j] );
            	tmp_val = (tmp_val << 4) ^ (hex_s);
            }
   			
   		}
   		
   		value[15+i] = tmp_val; 
   		//printf("%08x%08x ",(unsigned int)(value[15+i] >> 32), (unsigned int)(value[15+i] & 0xFFFFFFFFLU));
   	}
   	//printf("\n");
   	fgets(line, 100, f);
   	for(int i = 0; i < 5; i++){
   		char hex_s = 0;
   		UINT64 tmp_val = 0;
   		for(int j = 0; j < 17; j++){

            if(line[i*17 + j] <= '9' && line[i*17 + j] >= '0'){
                hex_s = line[i*17 + j] - '0';
            }else if(line[i*17 + j] <= 'z' && line[i*17 + j] >= 'a'){
                hex_s = line[i*17 + j] - 'a' + 10;
            }else if(line[i*17 + j] <= 'Z' && line[i*17 + j] >= 'A'){
                hex_s = line[i*17 + j] - 'A' + 10;
            }
            
            if(j < 16){
            	//printf("%c", line[i*17 + j] );
            	tmp_val = (tmp_val << 4) ^ (hex_s);
            }
   			
   		}
   		
   		value[20+i] = tmp_val; 
   		//printf("%08x%08x ",(unsigned int)(value[20+i] >> 32), (unsigned int)(value[20+i] & 0xFFFFFFFFLU));
   	}
   	
   	//printf("\n");

    
    //set the value of messaged state
    value[5] = ~value[20];
    value[6] = ~value[21];
    value[7] = ~value[22];
    value[8] = ~value[23];
    value[9] = ~value[24];
    
    value[15] = value[5];
    value[16] = value[6];
    //value[17] = value[17] & (0x7FFFFFFFFFFFFFFFLU) | (value[22] & ((UINT64)1<<63));
    value[17] = (value[17] & ((UINT64) 1 << 63)) ^ ((UINT64) 1 << 63); 
    value[17] ^= (~value[22]) & (0x7FFFFFFFFFFFFFFFLU);

    printf("2.compute the bitwise sum of last 320 bits\n");
    /*
    printf("messaged state:\n");
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            printf("%016lx ", value[i * 5 + j]);
        }
        printf("\n");
    }
    */


    UINT64 sum = 0;
    for(int i = 0; i < 5; i ++){
         sum ^= value[20+i];
    }
    
    printf("sum : %016lx\n", sum);
	sum ^= sum >> 32;
	sum ^= sum >> 16;
	sum ^= sum >> 8;
	sum ^= sum >> 4;
	sum ^= sum >> 2;
	sum ^= sum >> 1;
	bool flag = (UINT64) 1 & sum;
	if(flag){
	    printf("the bitwise sum of last 320 bits is 1\n");
	}else{
	    printf("the bitwise sum of last 320 bits is 0\n");
	}
	

	printf("3.compute values of 5 CPs\n");
	
	UINT64 cp[5];
	
	for(int i = 0; i < 5; i++){
	    cp[i] = ~(value[20+i]);
	}
	

	if(flag){
	 
	    UINT64 diff_pad = (value[22] ^ value[17] ) & ((UINT64)1<<63);
	    UINT64 diff0 = value[23] ^ value[18];
	    UINT64 diff1 = value[24] ^ value[19];
	    
	    if(diff_pad == 0){
	        //printf("22:%016lx, 17:%016lx xor:%016lx\n", value[22], value[17],value[22] ^ value[17]);
	        //printf("pad bit is %016lx\n", diff_pad);
	        cp[2] ^= diff_pad;
	        value[7] ^= diff_pad;
	    }else{
	        int pos = 0;
	        while((diff0 >> pos) & (UINT64)1 != 0 && pos < 64){	            
	            pos++;
	        }
	        
	        //printf("pos3:%d\n", pos);
	        if(pos < 64){
	            cp[3] ^= ((UINT64)1 << pos);
	            value[8] ^= ((UINT64)1 << pos);
	        }else{
	            pos = 0;
	            while((diff1 >> pos) & (UINT64)1 != 0 && pos < 64){	            
	                pos++;
	            }
	            //printf("pos4:%d\n", pos);
	            if(pos < 64){
	                cp[4] ^= ((UINT64)1 << pos);
	                value[9] ^= ((UINT64)1 << pos);
	            }
	        }
	        
	        
	        
	        
	    }
	    
	}
    FILE *messaged_file = fopen("../data/state_files/messaged_state.txt", "w+");
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            fprintf(messaged_file, "%016lx ", value[i * 5 + j]);
        }
        fprintf(messaged_file, "\n");
    }
    fclose(messaged_file);

    //todo write files for check

	printf("sum of 5 CPs:\n");
	printf("%016llx %016llx %016llx %016llx %016llx \n", cp[0], cp[1], cp[2], cp[3], cp[4]);
	printf("after theta:\n%016llx %016llx %016llx %016llx %016llx \n", cp[0] ^ value[20], cp[1] ^ value[21], cp[2] ^ value[22], cp[3] ^ value[23], cp[4] ^ value[24]);

	printf("4.generate equations\n");
	
	FILE *euq_file = fopen("../data/equ_files/equations_online.txt", "w+");
	
	char state[Five][Five][LANE][equLength];
	bool vars[Five * Five]={1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};  //1 means this lane is unknown,0 means constants
	//initial
	UINT64 value_tmp[5][5];
	for(int i = 0; i < 5; i++){
	    for(int j = 0; j < 5; j++){
	        value_tmp[i][j] = value[i * 5 + j];
	        //printf("%016lx ", value_tmp[i][j]);
	    }
	    //printf("\n");
	    
	}
	
	initState(state, value_tmp);
    initVars(state, vars);
    printf("generate 320 linear constraints in the first rounds.\n");
    writeSumOf5CPs(euq_file, state, cp);
    //printState(state);
    theta_b2r1(state, cp);

    
    rho(state);
    pi(state);
    chi(state);
    iota(state, 0);
    //generate 65-n guessed variables
    int start_param = PARAM_ST;
    //printState(state);
    
    printf("65-n guessed equations.\n");
    int n = writeGuessedVariables(euq_file, state, start_param);
    printf("n:%d\n", n);
    start_param += n;

    //UINT64 sum1[Five] = {2,2,2,0,1};
    UINT64 sum2[Five] = {2,2,2,2,2};
    
    printf("generate 128 linear constraints in the second rounds.\n");
    //generate linear constains of two cp, include 32 guessed linear equations.
    n = writeSumOf2CPs(euq_file, state, start_param);
    

    start_param += n;
    theta(state, sum2); //nt sum1 for output linear,sum2 for generate equations
    rho(state);
    pi(state);
    for (int x= 0; x < Five; x ++) {
        for (int y = 0; y < Five; y ++) {
            for (int z = 0; z < LANE; z++) {
                eliminateOne(state[x][y][z]);
            }
        }
    }
    chi(state);
    iota(state, 1);
    //printf("round1 end!\n");
    
    theta(state, sum2);
    rho(state);
    pi(state); 
    
    printf("generate 192 linear equations.\n");

    //printf("finish\n");
    FILE* relationship_file = fopen("../data/equ_files/192LinearRelationship.txt", "r+");
    printLinearRelationship(euq_file, relationship_file, state);

    
    printf("The number of guessed equations is %d.\n", start_param - PARAM_ST);
    printf("***Please replace the #define part in convert.h in '2.process_equ_online' and GPU source **\n");
    printf("#define PARAM_NUM %d", start_param - PARAM_ST);
    printf("\n*******************************************************************************************\n");

	fclose(relationship_file);
    fclose(	euq_file );
   	
    fclose(f);
    return 0;
}

