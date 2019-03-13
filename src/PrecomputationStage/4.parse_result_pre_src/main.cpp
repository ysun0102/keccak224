



#include "zdd.h"
#include "keccakP1600.h"


void convert_res_to_polys(FILE *f){
    FILE *solve_mat_result_file = fopen("../data/mat_files/solve_mat_result.txt", "r+");
    FILE *mapping_column_file = fopen("../data/mat_files/mapping_column_pre.txt", "r+");
    
    CHECK_NN(solve_mat_result_file);
    CHECK_NN(mapping_column_file);
    value_t val = 0, sol = 0;
    int n = 0;
    
    char line[100];
    while(!feof(solve_mat_result_file)){
        fgets(line, 100, solve_mat_result_file);
        int tmp_n = (line[2] - '0') * 10 + line[3] - '0';
        UINT64 tmp_val = 0, tmp_sol = 0;

        int eq_count = 0, pos = 0;
        while(line[pos] != 'x'){
            pos++;
        }
        pos++;
        do{
            //printf("%c ", line[pos]);
            
            value_t hex_s = 0;
            if(line[pos] <= '9' && line[pos] >= '0'){
                hex_s = line[pos] - '0';
            }else{
                hex_s = line[pos] - 'a' + 10;
            }
            tmp_val = (tmp_val << 4) ^ (hex_s);
            //printf("tmp:%x, hex_s:%x\n",tmp_val, hex_s);
            pos++;
        }while(line[pos] != ',');
        
        
        while(line[pos] != 'x'){
            pos++;
        }
        pos++;
        do{
            //printf("%c ", line[pos]);
            value_t hex_s = 0;
            if(line[pos] <= '9' && line[pos] >= '0'){
                hex_s = line[pos] - '0';
            }else{
                hex_s = line[pos] - 'a' + 10;
            }
            tmp_sol = (tmp_sol << 4) ^ (hex_s);
            //printf("tmp:%08x%08x, hex_s:%x\n",(unsigned int)(tmp_sol >> 32), (unsigned int) tmp_sol &0xFFFFFFFFULL,  hex_s);
            pos++;
        }while(line[pos] != '\n');
        
        
        //printf("\n");
        if(tmp_n > n){
            n = tmp_n;
            val = tmp_val;
            sol = tmp_sol;
            tmp_val = 0;
            tmp_sol = 0;
            printf("n = %d, val = %08x%08x, sol = %08x%08x\n", n, (unsigned int)(val >> 32), (unsigned int)val& 0xFFFFFFFFULL, (unsigned int)(sol >> 32), (unsigned int)sol& 0xFFFFFFFFULL);
        } 
        //printf("%s", line);
    }
    
    
    

    
    FILE* ori_file = fopen("../data/equ_files/equations_pre.txt", "r+");
    void *buf;
    int line_num = 0;
    char ori_line[200];
    while(!feof(ori_file) && line_num < 454){
    
        fgets(ori_line, 200, ori_file);
        fprintf(f, "%s", ori_line);
        line_num++;
    }



	//solutions
	char mc_line[500];
	char delims[] = " x";
    fgets(mc_line, 500, mapping_column_file);
    char *idx = NULL;
    idx = strtok( mc_line, delims );
    int t = 0;
   	while( idx != NULL && t < 64 ) {
       	idx = strtok( NULL, delims );
       	fprintf(f, "x%s", idx );
       	if((sol >> t) & (value_t)1){
			fprintf(f, "+1,\n");       
       
       	}else{
       		fprintf(f, ",\n");
       	
       	}
       	t++;
   	}  
    
    
    
    //guess values
    for(int i = 0; i < 63; i++){
        if((val >> i) & (value_t)1){
            fprintf(f, "x%d+1,\n", 600+i);
        }else{
            fprintf(f, "x%d,\n", 600+i);
        }
    }
    if((val >> 63) & (value_t)1){
    	fprintf(f, "x%d+1.\n", 663);
    }else{
    	fprintf(f, "x%d.\n", 663);
    }
    
   
    //printf("test:%#x",)
    
    fclose(solve_mat_result_file);
    fclose(mapping_column_file);
}




int main(int argc, const char * argv[]) {
    
        
    /**
     **********************************
     ** scan the input information
     **********************************
     */


    
    INFO_PR("Reading input polynomials")
    

    //char name[15];
    
    FILE *equations_result_file = fopen("../data/equ_files/equations_result_pre.txt", "w+");


    //CHECK_NN(in)
    convert_res_to_polys(equations_result_file);
    
    fseek(equations_result_file, 0, 0);
    
    
    // initialize a zdd data structure.
    zddpoly_t *zddp = zddpoly_init();
    
    zddpoly_readfile(zddp, equations_result_file);
    fclose(equations_result_file);

    
    zddpoly_simplify(zddp);
    zddpoly_print(zddp, 2);
    
    
    printf("convet to state\n");
    UINT64 b1_state[25];
    UINT64 tmp_state[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    KeccakP1600_StaticInitialize();
    KeccakP1600_Initialize(b1_state);
    
    for(int i  = 0; i < 8; i++){
    	for(int j = 0; j < 64; j++){
    		if(zdd_linear_constant(zddp, zddp->linear[i*64+j])){
    			tmp_state[i] = (tmp_state[i]) ^ ((UINT64)1 << j);
    			//printf("1");
    		}else{
    			
    			//printf("0");
    		}	
    	}    
    }
    
    
    b1_state[0] = tmp_state[0];
    b1_state[2] = tmp_state[1];
    b1_state[5] = tmp_state[2];
    b1_state[7] = tmp_state[3];
	b1_state[10] = tmp_state[4];
    b1_state[12] = tmp_state[5];
    b1_state[15] = tmp_state[6];
    b1_state[17] = tmp_state[7];
    
    b1_state[1] = UINT64_MAX;
    b1_state[3] = UINT64_MAX;
    
    
    
    FILE *output_pre_file = fopen("../data/state_files/output_pre_file.txt", "w+");
    //todo  check
    fprintf(output_pre_file, "initial state of precomputation stage:\n");
    for(int i = 0; i < 5; i++){
    	fprintf(output_pre_file, "%08x%08x %08x%08x %08x%08x %08x%08x %08x%08x\n", (unsigned int)(b1_state[i * 5] >> 32),((unsigned int)b1_state[i * 5]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 1]>>32), ((unsigned int)b1_state[i * 5+1]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 2] >>32), ((unsigned int)b1_state[i * 5 + 2]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 3] >>32), ((unsigned int)b1_state[i * 5 + 3]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 4] >>32), ((unsigned int)b1_state[i * 5 + 4]& 0xFFFFFFFFULL));
    
    }
    
    

    KeccakF1600Permute(b1_state,3);
    fprintf(output_pre_file, "\n\n");
    
    
    fprintf(output_pre_file, "output state of precomputation stage:\n");
    for(int i = 0; i < 5; i++){
    	fprintf(output_pre_file, "%08x%08x %08x%08x %08x%08x %08x%08x %08x%08x\n", (unsigned int)(b1_state[i * 5] >> 32),((unsigned int)b1_state[i * 5]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 1]>>32), ((unsigned int)b1_state[i * 5+1]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 2] >>32), ((unsigned int)b1_state[i * 5 + 2]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 3] >>32), ((unsigned int)b1_state[i * 5 + 3]& 0xFFFFFFFFULL), (unsigned int)(b1_state[i * 5 + 4] >>32), ((unsigned int)b1_state[i * 5 + 4]& 0xFFFFFFFFULL));
    
    }
    
    //count the nonzero bits 
    UINT64 diff[2];
    //diff[0] = b1_state[15] ^ b1_state[20];
    //diff[1] = b1_state[16] ^ b1_state[21];
    //diff[2] = b1_state[17] ^ b1_state[22];
    diff[0] = b1_state[18] ^ b1_state[23];
    diff[1] = b1_state[19] ^ b1_state[24];
    UINT64 diff_pad = b1_state[17] ^ b1_state[22];
    
    int nonzero_count = 0;
    for(int i = 0; i < 2; i++){
    	for(int j = 0; j < 64; j++){
    		if((diff[i] >> j) & (UINT64)1){
    			nonzero_count++;
    		}
    	}
    
    }
        
    if (!((diff_pad >> 63) & 1)){
        nonzero_count++;
    }
    
    printf("n = %d\n", nonzero_count - 64);
    
    fclose(output_pre_file);
    exit(0);
    
    
    
    
}















































