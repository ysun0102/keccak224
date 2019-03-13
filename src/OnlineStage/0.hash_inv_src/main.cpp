#include <stdio.h>
#include <memory.h>

#define PARAM_ST 700
#define Five 5
#define LANE 64
#define equLength 1000
#define OUTPUT_LEN 224

#define index(x, y) (((x)%5)+5*((y)%5))
#define MODFive(x)  ((x+5)%5)
#define MODLane(x)  ((x+LANE)%LANE)
typedef unsigned long long UINT64;
typedef UINT64 tKeccakLane;

static const tKeccakLane KeccakRoundConstants[24] =
{
    0x0000000000000001,
    0x0000000000008082,
    0x800000000000808a,
    0x8000000080008000,
    0x000000000000808b,
    0x0000000080000001,
    0x8000000080008081,
    0x8000000000008009,
    0x000000000000008a,
    0x0000000000000088,
    0x0000000080008009,
    0x000000008000000a,
    0x000000008000808b,
    0x800000000000008b,
    0x8000000000008089,
    0x8000000000008003,
    0x8000000000008002,
    0x8000000000000080,
    0x000000000000800a,
    0x800000008000000a,
    0x8000000080008081,
    0x8000000000008080,
    0x0000000080000001,
    0x8000000080008008,
};




void write_224HashBack_file(FILE *f, char state[Five][Five][LANE][equLength], UINT64 hash[4]){
   	//printf("\n%08x%08x \n",(unsigned int)(hash[0] >> 32), (unsigned int)(hash[0] & 0xFFFFFFFFLU));
   	//printf("\n%08x%08x \n",(unsigned int)(hash[1] >> 32), (unsigned int)(hash[1] & 0xFFFFFFFFLU));
   	//printf("\n%08x%08x \n",(unsigned int)(hash[2] >> 32), (unsigned int)(hash[2] & 0xFFFFFFFFLU));  	
   	//printf("\n%08x%08x \n",(unsigned int)(hash[3] >> 32), (unsigned int)(hash[3] & 0xFFFFFFFFLU));  	
	int count = 0;
	for (int x = 0; x < Five; x++) {

            for (int z =0 ; z < LANE; z ++) {

                if(count < OUTPUT_LEN){
                	int n = 0;

                	while (state[x][0][z][n] != ':') {
                    	fprintf(f, "%c",state[x][0][z][n++]);
                	}

                	if((hash[count/LANE] >> (count % LANE)) & (UINT64)1){
                		fprintf(f, "+1");
                	
                	}
                	
                	if(count == OUTPUT_LEN - 1){
                	    fprintf(f, ".\n");
                	    
                	}else{
                	    fprintf(f, ",\n");
                	}
                }
                count++;
                
                
                
                
            }
        }

}

int main(){
 	
 	printf("read hash value file\n");
   	FILE *f = fopen("../../hash_value.txt", "r+");
   	
   	UINT64 hash[4] = {0, 0, 0, 0};
   	char line[20];

   	for(int i = 0; i < 4; i++){
   		fgets(line, 20, f);
   		printf("%d. %s\n", i, line); 
   		
   		
   		char hex_s = 0;
   		UINT64 tmp_val = 0;
   		for(int j = 0; j < 16; j++){

            if(line[j] <= '9' && line[j] >= '0'){
                hex_s = line[j] - '0';
            }else if(line[j] <= 'z' && line[j] >= 'a'){
                hex_s = line[j] - 'a' + 10;
            }else if(line[j] <= 'Z' && line[j] >= 'A'){
                hex_s = line[j] - 'A' + 10;
            }
            
            if(j < 16){
            	//printf("%c", line[i*17 + j] );
            	tmp_val = (tmp_val << 4) ^ (hex_s);
            }
   			
   		}
   		
   		hash[i] = tmp_val; 
   		//printf("%08x%08x",(unsigned int)(hash[i] >> 32), (unsigned int)(hash[i] & 0xFFFFFFFFLU));  	
   	}
 	fclose(f);
 	printf("XOR round constants\n");
 	
 	hash[0] ^= KeccakRoundConstants[2];
 	
 	printf("generate linear equations from hash values\n");
 	FILE *out = fopen("../data/equ_files/192LinearRelationship.txt","w+");
 	UINT64 hash_not[4];
 	for(int i = 0; i < 4 ; i ++){
 	    hash_not[i] = ~hash[i];
 	}
 	for(int sl = 0; sl < 32; sl ++){
 	    for(int i = 0; i < 3 ;i ++){
 	        fprintf(out,"x%d",PARAM_ST + i * 64 + sl);
 	        if((hash_not[i+1] >> sl) & ((UINT64)1)){
 	            fprintf(out, "+x%d", PARAM_ST + (i + 2) * 64 + sl);
 	        }
 	        
 	        if((hash[i] >> sl) & (UINT64)1){
 	            fprintf(out, "+1");
 	        }
 	        fprintf(out, ",\n");
 	    }
 	    
 	    UINT64 A = hash[0] ^ (hash_not[1] & hash[2]);
 	    fprintf(out, "x%d", PARAM_ST + 3 * 64 + sl);
 	    if((A >> sl) & (UINT64)1){
 	        fprintf(out, "+x%d+1", PARAM_ST +4 * 64 + sl);
 	    }
 	    if((hash[3] >> sl) & (UINT64)1){
 	        fprintf(out, "+1");
 	    }
 	    fprintf(out, ",\n");
 	    
 	}
 	
 	for(int sl = 32; sl < 64; sl++){
 	     for(int i = 0; i < 2 ;i ++){
 	        fprintf(out, "x%d",PARAM_ST + i * 64 + sl);
 	        if((hash_not[i+1] >> sl) & ((UINT64)1)){
 	            fprintf(out, "+x%d", PARAM_ST + (i + 2) * 64 + sl);
 	        }
 	        
 	        if((hash[i] >> sl) & (UINT64)1){
 	            fprintf(out, "+1");
 	        }
 	        if(sl == 63 && i == 1){
 	            fprintf(out, "...\n");
 	        }else{
 	            fprintf(out, ",\n");
 	        }
 	    }
 	}
 	
 	fclose(out);
 	
 	//printAllState(state);
 	
 	
 	
 	return 0;

}
