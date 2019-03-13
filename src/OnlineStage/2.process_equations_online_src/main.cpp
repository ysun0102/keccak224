#include "convert.h"



//***********  print to screen *****************//

void printBinary1(linearpart_t n){
    for (int i = 0; i < 64; i++) {
        printf("%lu",(n>> i) &(unsigned long)1 );
    }
}


void printBinary1_file(linearpart_t n,FILE *f){
    for (int i = 0; i < 64; i++) {
        fprintf(f,"%lu",(n>> i) &(unsigned long)1 );
    }
}

void printBinary2(squarepart_t n){
    for (int i = 0; i < 64; i++) {
        printf("%lu",(n>> i) &(unsigned long)1 );
    }
}

void printBinary2_file(squarepart_t n,FILE *f){
    for (int i = 0; i < 64; i++) {
        fprintf(f,"%lu",(n>>i) &(unsigned long)1 );
    }
    
}

void printBinary3_file(oripoly_t n,FILE *f){
    for (int i = 0; i < 64; i++) {
        fprintf(f,"%lu",(n>> i) &(unsigned long)1 );
    }
}

void printBinary3(oripoly_t n){
    for (int i = 0; i < 64; i++) {
        printf("%lu",(n>> i) &(unsigned long)1 );
    }
}



int main(int argc, const char * argv[]) {
     
    
    
 
    
    /**
     **********************************
     ** scan the input information
     **********************************
     */


    
    INFO_PR("Reading input polynomials")
    

    //char name[15];
    
    FILE *in = fopen("../data/equ_files/equations_online.txt", "r+");


    CHECK_NN(in)
    
    // initialize a zdd data structure.
    zddpoly_t *zddp = zddpoly_init();
    
    zddpoly_readfile(zddp, in);
    fclose(in);
    
  
    
    
    /**
     **********************************
     ** Reduced by ZDD
     **********************************
     */    
    INFO_PR("Pre-Reductions")
    
    zddpoly_simplify(zddp);
    
    FILE *total_linear_file = fopen("../data/equ_files/total_linear_file.txt", "w+");
    
    zdd_linear_poly_write_to_files(total_linear_file, zddp);
    
    fclose(total_linear_file);
    
    
    FILE *rename_file = fopen("../data/equ_files/rename_equations_online.txt", "w+");
    
    zdd_nonlinear_poly_write_to_files(rename_file, zddp);
    
    fclose(rename_file);
    
    
    

    zddpoly_free(zddp, 1);


    /**
     **********************************
     ** Convert to Matrix
     **********************************
     */    

    FILE *in_rename = fopen("../data/equ_files/rename_equations_online.txt", "r+");
    CHECK_NN(in_rename)
    
    if (!in_rename) {
     	printf("\n error in open file.\n");
     	exit(0);
    }
    
    // initialize a zdd data structure.
    zddp = zddpoly_init();
    zddpoly_readfile(zddp, in);
    
    zddpoly_print(zddp, 2);
    fclose(in_rename);
    
    // generate random matrices.
    const int para_num = PARAM_NUM;
    const int unknown_num = zddp -> appeared_vars - PARAM_NUM;
    const int poly_num = zddp -> size_of_polys;
   // const int unknown_num = VAR_NUM;
    //const int poly_num = LINEAR_NUM;
    const int ori_num = NONLINEAR_NUM;


    printf("para:%d\nunknown:%d\npoly:%d\n", para_num, unknown_num, poly_num);
    printf("\n*************Please replace the #define part in GPU source **************\n");
    printf("#define UNKNOWN_NUM %d \n#define POLY_NUM %d\n#define PARAM_NUM %d\n", unknown_num, poly_num, para_num);
    printf("\n*************************************************************************\n");
    
    
    //save column idx
    idrc_t linear_mapping[192];
    for(int i = 0; i < 192; i++){
        linear_mapping[i] = 0;
    }
    //idrc_t working_mapping[192];
    //idrc_t square_mapping[64];
    //idrc_t poly_mat[192];
    FILE *mapping_file = fopen("../data/mat_files/mapping_column_online.txt","w+");
    generate_mapping_files(mapping_file, zddp, linear_mapping);
    fclose(mapping_file);
   
    
    
    printf("open file\n");
    FILE *linear_file = fopen("../data/mat_files/linear_mat.txt", "w+");
    FILE *working_file = fopen("../data/mat_files/working_mat.txt", "w+");
    FILE *square_file = fopen("../data/mat_files/square_mat.txt", "w+");
    //FILE *poly_file = fopen("../poly_mat.txt", "w+"); 
    

    // save matrix 
    linearpart_t linear_mat[para_num][poly_num][3];// linear part can be unsigned int, if the unknowns are smaller than 31
    linearpart_t working_mat[poly_num][3]; // initialized as the const part of linear matrix. also used as the results of linear part.
    linearpart_t working_mat_copy[poly_num][3];
    
    squarepart_t square_mat[para_num][poly_num];
    squarepart_t const_mat[poly_num]; // used to compute the const part from square polys.
    
    oripoly_t polys[ori_num][para_num + unknown_num + 1][3];
    oripoly_t cstpoly[2];
    printf("convert to matries\n");
    matrices_t eval;
    matrices_init(&eval, zddp);
    
    printf("write to files\n");
    //write the matrix files    
    for (int i = 0; i < para_num; i++) {
    
        for (int j = 0; j < poly_num; j++) {
            linear_mat[para_num- i -1][j][0] = ((&eval)->base_mat[i][j][0])^(&eval)->quadratic_mat[j][i][para_num];
            linear_mat[para_num - i -1][j][1] = ((&eval)->base_mat[i][j][1]);
            linear_mat[para_num - i -1][j][2] = ((&eval)->base_mat[i][j][2]);
            
            printBinary1_file(linear_mat[para_num- i -1][j][0], linear_file);
            fprintf(linear_file, "|");
            printBinary1_file(linear_mat[para_num- i -1][j][1], linear_file);
            fprintf(linear_file, "|");
            printBinary1_file(linear_mat[para_num- i -1][j][2], linear_file);
            fprintf(linear_file, " ");

            
        }

        fprintf(linear_file, "\n");

    }
    

    
    for (int j = 0; j < poly_num; j++) {
    
        //printf("%d\n", (&eval)->quadratic_mat[j][para_num][para_num]);
        
        working_mat[j][0] = ((&eval)->constant_array[j][0])^((&eval)->quadratic_mat[j][para_num][para_num]);
        working_mat[j][1] = ((&eval)->constant_array[j][1]);
        working_mat[j][2] = ((&eval)->constant_array[j][2]);

        printBinary1_file( working_mat[j][0], working_file);
        fprintf(working_file, "|");
        printBinary1_file( working_mat[j][1], working_file);
        fprintf(working_file, "|");
        printBinary1_file( working_mat[j][2], working_file);
        fprintf(working_file, "\n");


    }
    

    
    
    for (int i = 0; i < para_num; i++) {
        
        for (int j = 0; j < poly_num; j++) {
            squarepart_t value = 0;
            for (int k = 0; k < para_num; k++) {
                value = (value<<1)^(squarepart_t)(&eval)->quadratic_mat[j][i][k];
            }
            square_mat[para_num-i-1][j] = value;
            
            printBinary2_file(square_mat[para_num-i-1][j], square_file);
            fprintf(square_file, " ");

            
        }

        fprintf(square_file, "\n");
    }
    
    fclose(linear_file);
    fclose(working_file);
	fclose(square_file);
	
	
	
	FILE *f_in = fopen("../data/equ_files/total_linear_file.txt", "r+");
	FILE *f_out = fopen("../data/mat_files/totalLinear_mat.txt", "w+");
	int p[256];
	for(int i = 0; i < 64; i++){
	    if(i < PARAM_NUM + 1){
	        p[i] = PARAM_ST + i;
	    }else{
	        p[i] = 0;
	    }
	}
	for(int i = 0; i < 192; i++){
	    p[64 + i] = linear_mapping[i];
	}
    write_total_linear_mat_to_file(f_in, PARAM_ST + PARAM_NUM, p, f_out);
    fclose(f_in);

    matrices_free(&eval, zddp);
	zddpoly_free(zddp, 1);
	exit(0);
    
    
    
    
}















































