#include "misc.h"


//! input var number for zdd
#define ZDD_INPUT_VARNUM (2048)
#define ZDD_INPUT_POLYNUM (4096)

#define PARAM_ST 650

// data structure

struct zddpoly_t {
    
    //! table.
    DdManager *zdd;
    
    //! polys in zdd.
    DdNode ** polys;
    
    //! linear polynomials.
    DdNode ** linear;
    
    //! index from old to new
    //mzp_t *old_to_new;
    idrc_t *old_to_new;
    
    //! index from new to old
    //mzp_t *new_to_old;
    idrc_t *new_to_old;
    
    //! number of polys
    idrc_t size_of_polys;
    
    //! number of linear polys
    var_subscript_t rank_of_linear;
    
    //! the numof variables that are NOT appear in polys.
    var_subscript_t deleted_vars;
    
    //! number of the appeared variables. excluding linear polynomials.
    var_subscript_t appeared_vars;
    
    //! the largest index of appear variable in nonlinear polynomials.
    var_subscript_t maxindex;

    //! padding
    char _padding[sizeof(void *) - sizeof(var_subscript_t) - sizeof(var_subscript_t)];
    
}; // 56





// -------------------------- func ---------------------------

static inline void zddpoly_check(zddpoly_t *zddp) {
    
    CHECK_NN(zddp)
    CHECK_NN(zddp->zdd)
    
    CHECK_NN(zddp->polys)
    CHECK_GE(zddp->size_of_polys, 0)
    
    CHECK_NN(zddp->linear)
    CHECK_GE(zddp->rank_of_linear, 0)
    
    for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++)
        CHECK_NN(zddp->linear[i])
    
    CHECK_NN(zddp->old_to_new)
    CHECK_EQ(zddp->old_to_new->length, ZDD_INPUT_VARNUM)
    
    CHECK_NN(zddp->new_to_old)
    CHECK_EQ(zddp->new_to_old->length, ZDD_INPUT_VARNUM)
    
    // for rename
    CHECK_GE(zddp->appeared_vars, 0)
    CHECK_LE(zddp->appeared_vars, ZDD_INPUT_VARNUM)
    
    CHECK_GE(zddp->maxindex, 0)
    CHECK_LT(zddp->maxindex, ZDD_INPUT_VARNUM)
    
    // for reduction
    CHECK_GE(zddp->deleted_vars, 0)
    CHECK_LE(zddp->deleted_vars, zddp->rank_of_linear)
}




// ------ zdd ------

DdNode *zdd_plus(DdManager *zdd, DdNode *P, DdNode *Q);
void zdd_print(DdManager *zdd, DdNode *p);
void zdd_print(DdManager *zdd, DdNode *p, idrc_t *old_to_new);


// --------- zdd poly -------

zddpoly_t *zddpoly_init();
zddpoly_t *zddpoly_copy(zddpoly_t *zddp);
void zddpoly_clear(zddpoly_t *zddp);
void zddpoly_free(zddpoly_t *zddp, flag_t flag);


void zddpoly_readfile(zddpoly_t *zddp, FILE *in);
void zddpoly_print(zddpoly_t *zddp, flag_t flag);
void zddpoly_rename(zddpoly_t *zddp);

void zddpoly_guessvar(zddpoly_t *zddp, var_subscript_t ind, char val);
void zddpoly_guesslinear(zddpoly_t *zddp, var_subscript_t *inds, idrc_t length, char val);

deg_t zddpoly_maxdegree(zddpoly_t *zddp);

void zdd_interreduce_linear(zddpoly_t *zddp);
void zdd_nonlinear_reduceby_linear(zddpoly_t *zddp);
void zddpoly_simplify(zddpoly_t *zddp);

void zdd_rename_write_to_files(FILE *f,zddpoly_t *zddp, DdManager *zdd, DdNode *p);
void zdd_nonlinear_poly_write_to_files(FILE* f, zddpoly_t *zddp);
void zdd_linear_poly_write_to_files( FILE *f, zddpoly_t *zddp);

