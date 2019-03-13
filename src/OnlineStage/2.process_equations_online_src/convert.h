

#include <stdio.h>
#include "zdd.h"

#define PARAM_NUM 36

#define NONLINEAR_NUM 0
#define PARAM_ST 650



typedef long value_t; // to save values of variables.

//typedef unsigned long constpart_t; // the part with no parameters.
typedef unsigned long linearpart_t; // to save 32 unknowns and 1 contants.
typedef unsigned long squarepart_t;

typedef unsigned long oripoly_t;





/// functions

static inline void binary_print(value_t val, int len) {
    
    for (int i = 0; i < len; i++) {
        if (val & ((value_t)1 << i)) {
            printf("1");
        } else {
            printf("0");
        }
        
        if ((i + 1) % 5 == 0) {
            printf(" ");
        }
    }
    
}


static inline int largestpos(value_t val, int len) {
    
    CHECK_GT(val, 0)
    
    for (int i = len - 1; i >= 0; i--) {
        if (val & ((value_t)1 << i)) {
            return i;
        }
    }
    
    return -1;
}



static inline void print_status(linearpart_t *status, int poly_num, int word_len) {
    
    for (int i = 0; i < poly_num; i++) {
        binary_print(status[i], word_len);
        
        if (i < poly_num - 1) {
            printf("\n");
        } else {
            printf("\n");
        }
    }
    
}




struct matrices_t{
    unsigned long ***base_mat;
    unsigned long **constant_array;
    bool ***quadratic_mat;
    //bool ***nonlinear_mat;
};

bool is_nonlinear_param(zddpoly_t *zddp, DdManager *zdd, DdNode *p);
void matrices_init(matrices_t *eval,zddpoly_t *zddp);
void zdd_to_column(zddpoly_t *zddp, matrices_t *eval,int col, DdManager *zdd, DdNode *p, idrc_t *old_to_new);


void generate_mapping_files(FILE* f, zddpoly_t *zddp, int linear_mapping[192]);
void write_total_linear_mat_to_file(FILE *f, int num_var, int p[256], FILE *out);
void matrices_free(matrices_t *eval,zddpoly_t *zddp);


