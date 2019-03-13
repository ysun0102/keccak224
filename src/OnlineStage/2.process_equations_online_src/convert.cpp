
#include "convert.h"


/**
 * read meaningful char from file.
 */
static inline void meaningchar(FILE *f, char *p_c) { //r
    do {
        fscanf(f, "%c", p_c);
    } while ((*p_c) != 'x' && !((*p_c) >= '0' && (*p_c) <= '9') && (*p_c) != '(' && (*p_c) != ')' && (*p_c) != '.' && (*p_c) != ';' && (*p_c) != ',' && (*p_c) != '+');
}


void matrices_calloc(matrices_t *eval,zddpoly_t *zddp){
    int poly_num = zddp -> size_of_polys;

    eval -> base_mat = (unsigned long ***) calloc(PARAM_NUM, sizeof(unsigned long**));
    for(int i = 0; i < PARAM_NUM; i ++){
        eval -> base_mat[i] = (unsigned long**) calloc(poly_num, sizeof(unsigned long*));
        for(int j = 0; j < poly_num; j++){
            eval -> base_mat[i][j] = (unsigned long *) calloc(3, sizeof(unsigned long));
        }
        
    }

    eval -> constant_array = (unsigned long**) calloc(poly_num, sizeof(unsigned long*));
    for(int i = 0 ; i < poly_num ; i++){
        eval -> constant_array[i] = (unsigned long*) calloc(3, sizeof(unsigned long));
    }

    eval -> quadratic_mat = (bool***) calloc (poly_num, sizeof (bool**));
    for(int i = 0; i < poly_num; i++){
        eval -> quadratic_mat[i] = (bool**) calloc (PARAM_NUM+1, sizeof (bool*));
        for(int j = 0; j < PARAM_NUM + 1; j++){
            eval -> quadratic_mat[i][j] = (bool*) calloc (PARAM_NUM+1, sizeof (bool));
        
        }
    }
    
    

}


void matrices_free(matrices_t *eval,zddpoly_t *zddp){
    int poly_num = zddp -> size_of_polys;
    
    for(int i = 0; i < PARAM_NUM; i ++){

        for(int j = 0; j < poly_num; j++){
            free(eval -> base_mat[i][j]);
        }
        free(eval -> base_mat[i]);
        
    }
    free(eval -> base_mat);
    
    
    
    for(int i = 0 ; i < 3 ; i++){
        free(eval -> constant_array[i]);
    }
    free(eval -> constant_array);
    

    for(int i = 0; i < poly_num; i++){

        for(int j = 0; j < PARAM_NUM + 1; j++){
            free(eval -> quadratic_mat[i][j]);
        
        }
        free(eval -> quadratic_mat[i]);
    }
    free(eval -> quadratic_mat);
    
}


void matrices_init(matrices_t *eval,zddpoly_t *zddp){
	    

    matrices_calloc(eval, zddp);
 
    int linear_num = 0;
    int nonlinear_num = 0;
    for (idrc_t i = 0; i < zddp->size_of_polys; i++){

        if(is_nonlinear_param(zddp, zddp->zdd, zddp->polys[i])){
            //zdd_to_matrix_total(eval, nonlinear_num++, zddp->zdd, zddp->polys[i], zddp->old_to_new);
            printf("nonlinear\n");
        }else{
            zdd_to_column(zddp, eval, linear_num++, zddp->zdd, zddp->polys[i], zddp->old_to_new);
            
        }
    
    }
    printf("linear:%d, nonlinear:%d\n", linear_num, nonlinear_num);

}


bool is_nonlinear_param(zddpoly_t *zddp, DdManager *zdd, DdNode *p){

    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    bool flag = false;
    
    int var_all = zddp -> appeared_vars;
    
    for (int i = 0; i< var_all + 1; i++) {
        if(p == zero || p == one){
            continue;
        }else if(p -> index < PARAM_NUM){
            p = cuddE(p);
            continue;
        
        }else{
            //printf("x%d*(\n", p->index);
            t = cuddT(p);
            if (t != one && t != zero){
                
                e = cuddE(t);
                while (e != zero && e != one) {
                    
                    //printf("+x%d",e->index);
                    if(e -> index > PARAM_NUM){
                        flag = true;
                        break;
                    }
                    e = cuddE(e);
                }
            
            }
            p = cuddE(p);
        }
        
        if(flag == true){
            break;
        }
    }
    return flag;
}




void zdd_to_column(zddpoly_t *zddp, matrices_t *eval,int col, DdManager *zdd, DdNode *p, idrc_t *old_to_new){
    
    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    unsigned long base[3] = {0,0,0};
    unsigned long constant[3] = {0,0,0};


    int var_all = zddp -> appeared_vars;
    
    int row = 0;
    for (int i = 0; i< var_all + 1; i++) {
        base[0] = 0;
        base[1] = 0;
        base[2] = 0;
        if (p == zero){
            continue;
        }else if (p == one){
            
            eval->quadratic_mat[col][PARAM_NUM][PARAM_NUM] = 1;
        }else{
            
            if (p->index< 100) {  //Ci*(Ci's+Xi's)
                
                row = old_to_new[p->index];
                
                t = cuddT(p);
                if(t!= zero && t!= one ){
                    if (t->index > 100) {   //Xi's
                        int len = var_all  - old_to_new[t->index];
                        int bn = len / 64;
                        int pos = len - bn * 64;

                        base[bn] ^= ((unsigned long)1 << pos);
                    }else{  //Ci's
                        
                        eval->quadratic_mat[col][row][old_to_new[t->index]] =1;
                    }
                    
                    e = cuddE(t);
                    while (e != zero && e != one) {
                                                      
                        if(e->index > 100){
                            int len = var_all  -old_to_new[e->index];
                            int bn = len / 64;
                            int pos = len - bn * 64;

                            base[bn] ^= ((unsigned long)1 << pos);

                            
                            
                        }else{
                                                      
                            eval->quadratic_mat[col][row][old_to_new[e->index]] =1;
                                                      
                        }
                        e = cuddE(e);
                    }
                    if (e == one) {
                        
                        //todo var_array
                        eval->quadratic_mat[col][row][PARAM_NUM] = 1;
                        
                        
                    }
                    
                }else if(t == one){
                    
                    //todo var_array
                    eval->quadratic_mat[col][row][PARAM_NUM] = 1;
                    
                    
                }else if (t == zero){
                    printf("\nZDD is WRONG!!!\n\n\n");
                }
                
                
                eval->base_mat[row][col][0] = base[0];
                eval->base_mat[row][col][1] = base[1];
                eval->base_mat[row][col][2] = base[2];

                
                
                
                
            }else{  //Xi*(Xi's)
                
                if(cuddT(p)!= one){
                    printf("%d not a linear polys after convert to mat!!!!\n\n", col);
                    
                    break;
                }else{
                    int len = var_all  -old_to_new[p->index];
                    int cn = len / 64;
                    int pos = len - cn * 64;
                    constant[cn] ^= ((unsigned long) 1 << pos);

                }
                
                
                
                
                
            }
            p = cuddE(p);
            
        }
        eval->constant_array[col][0] = constant[0];
        eval->constant_array[col][1] = constant[1];
        eval->constant_array[col][2] = constant[2];

    }
    
}



void generate_mapping_files(FILE* f, zddpoly_t *zddp, int linear_mapping[256]){
    
    const int unknown_num = zddp -> appeared_vars - PARAM_NUM;
    int var_all = zddp -> appeared_vars;
    //linear_mat
    fprintf(f, "linear/working_mat:");
    fprintf(f, "1 ");
    for(int i = 0; i < unknown_num; i++){
        fprintf(f, "x%d ", zddp->new_to_old[var_all - i - 1]);
        linear_mapping[i] = zddp->new_to_old[var_all - i - 1];
    }
    fprintf(f, "\n");

    //square_mat
    fprintf(f, "square_mat:");
    for(int i = 0; i < PARAM_NUM; i++){
        fprintf(f, "x%d ", zddp->new_to_old[PARAM_NUM - i - 1]);
    }
    
    
    fprintf(f, "\n");

}


void write_total_linear_mat_to_file(FILE *f, int num_var, int p[256], FILE *out){


    char c;
    meaningchar(f, &c);
    bool TLmat[num_var][num_var+1];
    for (int i = 0; i < num_var; i++) {
        for (int j = 0; j < num_var+1; j++) {
            TLmat[i][j] = 0;
        }
    }
    
    bool first = true;
    int row = 0;
    while (c != '.') {
        if(c == 'x'){
            
            int d = 0;
            meaningchar(f, &c);
            while (c <= '9' && c >= '0') {
                d = d * 10 + c - '0';
                meaningchar(f, &c);
            }
            
            if(first){
                first = false;
                if(row < d){
                    row++;
                    while (row < d) {
                        TLmat[row][row] = 1;
                        row++;
                    }
                }
            }else{
                TLmat[row][d] = 1;
            }
            
        }else if (c == ','){
            first = true;
            meaningchar(f, &c);
        }else if(c == '1'){
            TLmat[row][num_var] = 1;
            meaningchar(f, &c);
        }else{
            meaningchar(f, &c);
        }
        
    }
    
    for (int r = row + 1; r < num_var; r++) {
        TLmat[r][r] = 1;
    }

    
    for (int r = 0; r < 640; r ++) {
        for (int c =0; c < 256; c++) {
            if (c % 64 == 0) {
                fprintf(out, " ");
            }
            if(p[c] > 0){
                fprintf(out, "%d", TLmat[r][p[c]]);
            }else{
                fprintf(out, "0");
            }
            
        }
        fprintf(out, "\n");
    }
    
    
}


