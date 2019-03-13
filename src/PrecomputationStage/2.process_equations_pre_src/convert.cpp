
#include "convert.h"

void matrices_init(matrices_t *eval,zddpoly_t *zddp){
	    
    memset(eval->base_mat,0,sizeof(eval->base_mat));
    memset(eval->constant_array,0,sizeof(eval->base_mat));
    memset(eval->var_array,0,sizeof(eval->var_array));
    memset(eval->quadratic_mat,0,sizeof(eval->quadratic_mat));
    memset(eval->nonlinear_mat,0,sizeof(eval->nonlinear_mat));
 
 
    int linear_num = 0;
    int nonlinear_num = 0;
    for (idrc_t i = 0; i < zddp->size_of_polys; i++){

        if(is_nonlinear_param(zddp->zdd, zddp->polys[i])){
            zdd_to_matrix_total(eval, nonlinear_num++, zddp->zdd, zddp->polys[i], zddp->old_to_new);
            
        }else{
            zdd_to_column(eval, linear_num++, zddp->zdd, zddp->polys[i], zddp->old_to_new);
            
        }
    
    }
    printf("linear:%d, nonlinear:%d\n", linear_num, nonlinear_num);

}


bool is_nonlinear_param(DdManager *zdd, DdNode *p){

    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    bool flag = false;
    for (int i = 0; i< PARAM_NUM + VAR_NUM + 1; i++) {
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


void zdd_to_matrix_total(matrices_t *eval,int mid,DdManager *zdd, DdNode *p, idrc_t *old_to_new){
    
    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    int var_all = PARAM_NUM+VAR_NUM;
    int row = 0;
    for (int i = 0; i< PARAM_NUM + VAR_NUM + 1; i++) {
        
        if (p == zero){
            continue;
        }else if (p == one){
            
            eval->nonlinear_mat[mid][var_all][var_all] = 1;
        }else{
            row = old_to_new[p->index];
            
            t = cuddT(p);
            if(t == one){
               
                //todo var_array
                eval->nonlinear_mat[mid][row][var_all] = 1;
                
                
            }else if (t == zero){
                printf("\nZDD is WRONG!!!\n\n\n");
            }else{
                eval->nonlinear_mat[mid][row][old_to_new[t->index]] = 1;
                
                e = cuddE(t);
                
                while (e != zero && e != one) {
                    
                    eval->nonlinear_mat[mid][row][old_to_new[e->index]] =1;
                    
                    e = cuddE(e);
                }
                
                if (e == one) {
                    eval->nonlinear_mat[mid][row][var_all] = 1;
                }
            }
            p = cuddE(p);
        }
        
    }
    

}

void zdd_to_column(matrices_t *eval,int col, DdManager *zdd, DdNode *p, idrc_t *old_to_new){
    
    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    unsigned long base = 0;
    unsigned long constant = 0;
    int row = 0;
    for (int i = 0; i< PARAM_NUM + VAR_NUM + 1; i++) {
        base = 0;
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
                        
                        base ^= ((unsigned long)1 << (PARAM_NUM + VAR_NUM - 1 - old_to_new[t->index]));
                        
                    }else{  //Ci's
                        
                        eval->quadratic_mat[col][row][old_to_new[t->index]] =1;
                    }
                    
                    e = cuddE(t);
                    while (e != zero && e != one) {
                                                      
                        if(e->index > 100){
                            
                            base ^= ((unsigned long)1<< (PARAM_NUM + VAR_NUM - 1 -old_to_new[e->index]));
                            
                            
                            
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
                
                
                eval->base_mat[row][col] = base;
                
                
                
                
            }else{  //Xi*(Xi's)
                
                if(cuddT(p)!= one){
                    printf("not a linear polys after evaluation!!!!\n\n");
                    
                    break;
                }else{
                    
                    constant ^= ((unsigned long)1<< (PARAM_NUM + VAR_NUM - 1 -old_to_new[p->index]));
                }
                
                
                
                
                
            }
            p = cuddE(p);
            
        }
        eval->constant_array[col] = constant;
        
    }
    
}

void base_mat_print(matrices_t *eval){
    
    printf("\n------------ base  matrix ---------------\n");
    
    for (int i = 0; i< PARAM_NUM; i++) {
        for (int j = 0; j < LINEAR_NUM; j++) {
            
            printf("%lu ",eval->base_mat[i][j]);
        }
        printf("\n");
    }
    
    printf("\n------------ constant array ---------------\n");
    
    for (int i = 0; i < LINEAR_NUM; i++) {
        printf("%lu\n",eval->constant_array[i] );
    }
    
    printf("\n------------ quadratic matrices ---------------\n");
    for (int i = 0; i < LINEAR_NUM; i++) {
        printf("%d polys\n",i);
        for (int j = 0; j < PARAM_NUM+1; j++) {
            for (int k = 0; k < PARAM_NUM+1; k++) {
                printf("%d ",eval->quadratic_mat[i][j][k]);
            }
            printf("\n");
        }
    }
    
    printf("\n------------ nonlinear matrices ---------------\n");
    for (int i = 0; i < NONLINEAR_NUM; i++) {
        printf("%d nonlinear polys\n",i);
        for (int j = 0; j < PARAM_NUM+ VAR_NUM+1; j++) {
            for (int k = 0; k < PARAM_NUM +VAR_NUM +1; k++) {
                printf("%d ",eval->nonlinear_mat[i][j][k]);
            }
            printf("\n");
        }
    }

    
}


void generate_mapping_files(FILE* f, zddpoly_t *zddp){
    //linear_mat
    fprintf(f, "linear/working_mat:");
    fprintf(f, "1 ");
    for(int i = 0; i < VAR_NUM; i++){
        fprintf(f, "x%d ", zddp->new_to_old[PARAM_NUM + VAR_NUM - i - 1]);
    }
    fprintf(f, "\n");

    //square_mat
    fprintf(f, "square_mat:");
    for(int i = 0; i < PARAM_NUM; i++){
        fprintf(f, "x%d ", zddp->new_to_old[PARAM_NUM - i - 1]);
    }
    
    
    fprintf(f, "\n");
    //polys_mat
    fprintf(f, "nonlinear_mat:");
    for(int i = 0 ; i < PARAM_NUM ; i++){
        fprintf(f, "x%d ", zddp->new_to_old[PARAM_NUM - i - 1]);
    }
    
    for(int i = 0 ; i < VAR_NUM; i++){
        fprintf(f, "x%d ", zddp->new_to_old[PARAM_NUM +VAR_NUM - i - 1]);
    }
    
    fprintf(f, "1\n");
}
