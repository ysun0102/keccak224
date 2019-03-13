#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long UINT64;
typedef UINT64 tKeccakLane;

void read_line(tKeccakLane value[5], char line[100]){

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
            	//printf("%c/%01x  ", line[i*17 + j], hex_s );
            	tmp_val = (tmp_val << 4) ^ (hex_s);
            }
   			
   		}
   		
   		value[i] = tmp_val; 
   		//printf("%08x%08x ",(unsigned int)(value[15+i] >> 32), (unsigned int)(value[15+i] & 0xFFFFFFFFLU));
   	}
   	
}


int main(){


    printf("parse the result\n");
    tKeccakLane init_state_pre[25], output_state_pre[25], messaged_state_online[25],  init_state_online[25], preimage_message_2blocks[36];
    
    FILE *pre_file = fopen("../../PrecomputationStage/data/state_files/output_pre_file.txt", "r+");
    FILE *online_file = fopen("../data/state_files/final_messaged_state_online.txt", "r+");
    
    char line[100];
    
    //read the initial state of 1st block
    fgets(line, 100, pre_file);
    for(int i = 0; i < 5; i++){
   		fgets(line, 100, pre_file);
   		read_line(init_state_pre + i * 5, line);   	
   	}
   	
   	while(line[0]!= 'o'){
   	   	fgets(line, 100, pre_file);
   	}

   	//read the output state of 1st block
   	for(int i = 0; i < 5; i++){
   		fgets(line, 100, pre_file);
   		read_line(output_state_pre + i * 5, line);   	
   	}
   	fclose(pre_file);
   	
   	
   	
   	//read the message state of the 2nd block
   	fgets(line, 100, online_file);
    for(int i = 0; i < 5; i++){
   		fgets(line, 100, online_file);
   		printf("%s\n" , line);
   		read_line(messaged_state_online + i * 5, line);   	
   	}
   	
   	//compute the initial state of 2nd block
   	for(int i= 0; i < 25; i++){
   	    init_state_online[i] = messaged_state_online[i] ^ output_state_pre[i]; 
   	}
   	
   
   	
   	//generate preimage
   	for(int i= 0; i < 18 ; i++){
   	    preimage_message_2blocks[i] = init_state_pre[i];
   	    preimage_message_2blocks[18+i] = init_state_online[i];
   	    
   	}
   	
   	//check padding
   	if(preimage_message_2blocks[35] >>63 & 1){
   	    printf("Message is legal.\n");
   	}else{
   	    printf("Message is illegal.\n");
   	    exit(0);
   	}
   	
   	//compute length

   	int pos = -1;
   	for(int i = 1 ;i < 64 ;i++){
   	    if(preimage_message_2blocks[35] & (UINT64)1 << (63 - i)){
   	        pos = i;
   	        break;
   	    }
   	}
   	
   	bool flag = false;
   	if(pos < 0){
   	    for(int i= 2;i <36;i++){
   	        for(int j = 0 ; j < 64; j++){
   	            if(preimage_message_2blocks[36 - i] & (UINT64)1 << (63 - j)){
   	                pos = i * 64 + j;
   	                flag = true;
   	                break;
   	            }
   	        }
   	        if(flag){
   	            break;
   	        }
   	    }   	
   	}
   	
   	if(pos < 0){
   	    printf("Message is illegal.\n");
   	    exit(0);
   	}else{
   	    FILE *preimage_file = fopen("../preimage_2blocks.txt", "w+");
   	    int length = 2304 - pos - 1;
   	    printf("The length of the message is %d\n", length);
   	    fprintf(preimage_file, "The length of the message is %d.\n", length);
   	  	for(int i= 0; i < 35; i++){
   	       printf("0x%016llX, ", preimage_message_2blocks[i]);
   	       fprintf(preimage_file, "0x%016llX ", preimage_message_2blocks[i]);
   	    }
   	    printf("0x%016llX \n", preimage_message_2blocks[35]);
   	    fprintf(preimage_file, "0x%016llX \n", preimage_message_2blocks[35]);
   	    
   	    fclose(preimage_file);
   	}
   	
   	fclose(online_file);
    
    
}
