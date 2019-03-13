


#include "zdd.h"



/**
 * initialize the zdd structure.
 */
zddpoly_t *zddpoly_init() //r
{
    
    zddpoly_t *zddp = (zddpoly_t *) gb_calloc(1, sizeof(zddpoly_t));
    CHECK_MEM(zddp)
    
    DdManager *zdd = Cudd_Init(ZDD_INPUT_VARNUM, ZDD_INPUT_VARNUM, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    Cudd_AutodynDisableZdd(zdd);
    zddp->zdd = zdd;
    
    
    zddp->polys = (DdNode **) gb_calloc(ZDD_INPUT_POLYNUM, sizeof(DdNode *));
    CHECK_MEM(zddp->polys);
    
    zddp->linear = (DdNode **) gb_calloc(ZDD_INPUT_VARNUM, sizeof(DdNode *));
    CHECK_MEM(zddp->linear);
    
    //zddp->old_to_new = mzp_init(ZDD_INPUT_VARNUM);
    //zddp->new_to_old = mzp_init(ZDD_INPUT_VARNUM);
    zddp->old_to_new = (idrc_t*)gb_calloc(ZDD_INPUT_VARNUM, sizeof(idrc_t));
    zddp->new_to_old = (idrc_t*)gb_calloc(ZDD_INPUT_VARNUM, sizeof(idrc_t));    
    
    zddp->size_of_polys = 0;
    zddp->rank_of_linear = 0;
    zddp->deleted_vars = 0;
    
    zddp->appeared_vars = 0;
    zddp->maxindex = 0;
    
    return zddp;
}






/**
 * copy a zdd structure.
 */
zddpoly_t *zddpoly_copy(zddpoly_t *oldzddp) //r
{
#if DEBUG_MODE
    zddpoly_check(oldzddp);
#endif
    
    
    zddpoly_t *zddp = (zddpoly_t *) gb_calloc(1, sizeof(zddpoly_t));
    CHECK_MEM(zddp)
    
    
    zddp->zdd = oldzddp->zdd;
    
    
    zddp->polys = (DdNode **) gb_calloc(ZDD_INPUT_POLYNUM, sizeof(DdNode *));
    CHECK_MEM(zddp->polys);
    for (idrc_t i = 0; i < oldzddp->size_of_polys; i++) {
        zddp->polys[i] = oldzddp->polys[i];
        CHECK_NN(zddp->polys[i])
        Cudd_Ref(zddp->polys[i]);
    }

    
    zddp->linear = (DdNode **) gb_calloc(ZDD_INPUT_VARNUM, sizeof(DdNode *));
    CHECK_MEM(zddp->linear);
    for (var_subscript_t i = 0; i < oldzddp->rank_of_linear; i++) {
        zddp->linear[i] = oldzddp->linear[i];
        CHECK_NN(zddp->linear[i])
        Cudd_Ref(zddp->linear[i]);
    }
    
    
    zddp->old_to_new = (idrc_t*)gb_calloc(ZDD_INPUT_VARNUM, sizeof(idrc_t));
    for (var_subscript_t i = 0; i < ZDD_INPUT_VARNUM; i++)
        zddp->old_to_new[i] = oldzddp->old_to_new[i];
    
    
    zddp->new_to_old = (idrc_t*)gb_calloc(ZDD_INPUT_VARNUM, sizeof(idrc_t));
    for (var_subscript_t i = 0; i < ZDD_INPUT_VARNUM; i++)
        zddp->new_to_old[i] = oldzddp->new_to_old[i];
    
    
    zddp->size_of_polys = oldzddp->size_of_polys;
    zddp->rank_of_linear = oldzddp->rank_of_linear;
    zddp->deleted_vars = oldzddp->deleted_vars;
    
    zddp->appeared_vars = oldzddp->appeared_vars;
    zddp->maxindex = oldzddp->maxindex;
    
    return zddp;
}







/**
 * clear zdd structure.
 */
void zddpoly_clear(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
        if (zddp->polys[i]) {
            Cudd_RecursiveDerefZdd(zddp->zdd, zddp->polys[i]);
            zddp->polys[i] = NULL;
        }
    }
    
    
    for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
        
        CHECK_NN(zddp->linear[i])
        
        Cudd_RecursiveDerefZdd(zddp->zdd, zddp->linear[i]);
        zddp->linear[i] = NULL;
    }
    
    zddp->size_of_polys = 0;
    zddp->rank_of_linear = 0;
    zddp->deleted_vars = 0;
    
    zddp->appeared_vars = 0;
    zddp->maxindex = 0;
    
    
#if DEBUG_MODE
    printf("non zero reference of dd is %d .\n\n", Cudd_CheckZeroRef(zddp->zdd));
#endif
}



/**
 * free zdd structure.
 * all DISAPPEAR.
 * flag == 0, not quit cudd.
 * flag == 1, quit cudd.
 */
void zddpoly_free(zddpoly_t *zddp, flag_t flag) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
        
        CHECK_NN(zddp->polys[i])
        
        Cudd_RecursiveDerefZdd(zddp->zdd, zddp->polys[i]);
        zddp->polys[i] = NULL;
    }
    
    gb_free(zddp->polys);
    zddp->polys = NULL;
    
    
    for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
        
        CHECK_NN(zddp->linear[i])
        
        Cudd_RecursiveDerefZdd(zddp->zdd, zddp->linear[i]);
        zddp->linear[i] = NULL;
    }
    
    gb_free(zddp->linear);
    zddp->linear = NULL;
    
    gb_free(zddp->old_to_new);
    zddp->old_to_new = NULL;
    gb_free(zddp->new_to_old);
    zddp->new_to_old = NULL;
    
    zddp->size_of_polys = 0;
    zddp->rank_of_linear = 0;
    zddp->deleted_vars = 0;
    
    zddp->appeared_vars = 0;
    zddp->maxindex = 0;
    
    
#if DEBUG_MODE
    printf("non zero reference of dd is %d .\n\n", Cudd_CheckZeroRef(zddp->zdd));
#endif
    
    if (flag)
        Cudd_Quit(zddp->zdd);
    
    gb_free(zddp);
}











//////////////////////// auxilliary funcitons /////////////////////////

static inline bool zdd_be_linear(DdManager *zdd, DdNode *p) //r
{
    DdNode *t = p;
    
    while (t != DD_ONE(zdd) && t != DD_ZERO(zdd) && cuddT(t) == DD_ONE(zdd))
        t = cuddE(t);
    
    if (t == DD_ONE(zdd) || t == DD_ZERO(zdd)) {
        return true;
    } else
        return false;
}



static inline deg_t zdd_degree(DdManager *zdd, DdNode *p) //r
{
    CHECK_NN(zdd)
    CHECK_NN(p)
    
    DdNode *zero = DD_ZERO(zdd);
    DdNode *one = DD_ONE(zdd);
    
    
    if (p == zero) {
        return -1;
    }
    
    if (p == one) {
        return 0;
    }
    
    deg_t dt = (cuddT(p) == one) ? 1 : zdd_degree(zdd, cuddT(p)) + 1;
    deg_t de = zdd_degree(zdd, cuddE(p));
    
    CHECK_GE(GB_MAX(dt, de), 0)
    
    return GB_MAX(dt, de);
}



/**
 * plus two zdd P and Q.
 * P and Q are zdd or null.
 * never return null.
 * Add reference to the result.
 * P and Q are unchanged.
 */
DdNode *_zdd_plus(DdManager *zdd, DdNode *P, DdNode *Q) //r
{
    var_subscript_t p_top, q_top;
    DdNode *zero = DD_ZERO(zdd);
    DdNode *t, *e, *res, *pt, *pe, *qt, *qe;
    DdManager *table = zdd;
    
    
    //trivial cases
    statLine(zdd);
    
    if (P == NULL && Q == NULL) {
        Cudd_Ref(zero);
        return(zero);
    }
    if (P == zero || P == NULL) {
        Cudd_Ref(Q);
        return(Q);
    }
    if (Q == zero || Q == NULL) {
        Cudd_Ref(P);
        return(P);
    }
    if (P == Q) {
        Cudd_Ref(zero);
        return(zero);
    }
    
    
    //check cache
    res = cuddCacheLookup2Zdd(table, _zdd_plus, P, Q);
    if (res != NULL) {
        Cudd_Ref(res);
        return(res);
    }
    
    
    //compute
    p_top = P->index;
    q_top = Q->index;
    if (p_top < q_top) {
        
        pt = cuddT(P);
        pe = cuddE(P);
        
        e = _zdd_plus(zdd, pe, Q);
        res = cuddZddGetNode(zdd, P->index, pt, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, e);
        
    } else if (p_top > q_top) {
        
        qt = cuddT(Q);
        qe = cuddE(Q);
        
        e = _zdd_plus(zdd, P, qe);
        res = cuddZddGetNode(zdd, Q->index, qt, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, e);
        
    } else {
        
        pt = cuddT(P);
        pe = cuddE(P);
        qt = cuddT(Q);
        qe = cuddE(Q);
        
        t = _zdd_plus(zdd, pt, qt);
        e = _zdd_plus(zdd, pe, qe);
        res = cuddZddGetNode(zdd, P->index, t, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, t);
        Cudd_RecursiveDerefZdd(zdd, e);
    }
    
    cuddCacheInsert2(table, _zdd_plus, P, Q, res);
    
    CHECK_NN(res)
    
    return(res);
}


DdNode *zdd_plus(DdManager *zdd, DdNode *P, DdNode *Q) //r
{
    DdNode *res;
    
    do {
        zdd->reordered = 0;
        res = _zdd_plus(zdd, P, Q);
    } while (zdd->reordered == 1);
    
    CHECK_NN(res)
    
    return(res);
}



/**
 * P times Q.
 * P and Q are zdd or null.
 * never return null. null is treated as zero.
 * Add reference to the result.
 * P and Q are unchanged.
 */
DdNode *_zdd_times(DdManager *zdd, DdNode *P, DdNode *Q) //r
{
    var_subscript_t p_top, q_top;
    DdNode *zero = DD_ZERO(zdd);
    DdNode *one = DD_ONE(zdd);
    DdNode *t, *e, *res, *pt, *pe, *qt, *qe, *t1, *t2, *t3, *t4;
    DdManager *table = zdd;
    
    
    //trivial cases
    statLine(zdd);
    
    if (P == zero || P == NULL || Q == zero || Q == NULL) {
        Cudd_Ref(zero);
        return(zero);
    }
    if (P == one) {
        Cudd_Ref(Q);
        return(Q);
    }
    if (Q == one) {
        Cudd_Ref(P);
        return(P);
    }
    if (P == Q) {
        Cudd_Ref(P);
        return(P);
    }
    
    
    //check cache
    res = cuddCacheLookup2Zdd(table, _zdd_times, P, Q);
    if (res != NULL) {
        Cudd_Ref(res);
        return(res);
    }
    
    //compute by myself
    p_top = P->index;
    q_top = Q->index;
    if (p_top < q_top) {
        
        pt = cuddT(P);
        pe = cuddE(P);
        
        t = _zdd_times(zdd, pt, Q);
        e = _zdd_times(zdd, pe, Q);
        res = cuddZddGetNode(zdd, P->index, t, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, t);
        Cudd_RecursiveDerefZdd(zdd, e);
        
    } else if (p_top > q_top) {
        
        qt = cuddT(Q);
        qe = cuddE(Q);
        
        t = _zdd_times(zdd, P, qt);
        e = _zdd_times(zdd, P, qe);
        res = cuddZddGetNode(zdd, Q->index, t, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, t);
        Cudd_RecursiveDerefZdd(zdd, e);
        
    } else {
        
        pt = cuddT(P);
        pe = cuddE(P);
        
        qt = cuddT(Q);
        qe = cuddE(Q);
        
        t1 = _zdd_times(zdd, pt, qt);
        t2 = _zdd_times(zdd, pt, qe);
        t3 = _zdd_times(zdd, pe, qt);
        t4 = zdd_plus(zdd, t2, t3);
        t = zdd_plus(zdd, t1, t4);
        e = _zdd_times(zdd, pe, qe);
        res = cuddZddGetNode(zdd, P->index, t, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, t1);
        Cudd_RecursiveDerefZdd(zdd, t2);
        Cudd_RecursiveDerefZdd(zdd, t3);
        Cudd_RecursiveDerefZdd(zdd, t4);
        Cudd_RecursiveDerefZdd(zdd, t);
        Cudd_RecursiveDerefZdd(zdd, e);
        
    }
    
    cuddCacheInsert2(table, _zdd_times, P, Q, res);
    
    CHECK_NN(res)
    
    return(res);
}


DdNode *zdd_times(DdManager *zdd, DdNode *P, DdNode *Q) //r
{
    DdNode *res;
    
    do {
        zdd->reordered = 0;
        res = _zdd_times(zdd, P, Q);
    } while (zdd->reordered == 1);
    
    CHECK_NN(res)
    
    return(res);
}








/**
 * recursive function that reads a part each time and returns a single polynomial
 * add reference to the result
 * never return NULL, retuns 0 instead.
 */
DdNode *_zdd_readfile2(DdManager *zdd, FILE *f, bool *p_end) //r
{
    var_subscript_t d;
    char c;
    DdNode *res, *p, *prv, *tp;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    res = zero;
    Cudd_Ref(res);
    
    fscanf(f, "%c", &c);
    while (c != 'x' && c != '1' && c != '(')
        fscanf(f, "%c", &c);
    
    while (c != '.' && c != ')' && c != ';' && c != ',') {
        
        p = one;
        Cudd_Ref(p);
        
        while (c == '+' || c == '\n')
            fscanf(f, "%c", &c);
        
        if (c != '1') {
            do {
                if (c == '(') {
                    
                    prv = _zdd_readfile2(zdd, f, p_end);
                    tp = zdd_times(zdd, p, prv);
                    Cudd_RecursiveDerefZdd(zdd, prv);
                    Cudd_RecursiveDerefZdd(zdd, p);
                    p = tp;
                    fscanf(f, "%c", &c);
                    
                } else if (c == 'x') {
                    
                    d = 0;
                    do
                        fscanf(f, "%c", &c);
                    while (c == '[' || c == '\n');
                    
                    while (c >= '0' && c <= '9') {
                        d = d * 10 + (c - '0');
                        do
                            fscanf(f, "%c", &c);
                        while (c == '\n');
                    }
                    
                    while (c == ']' || c == '\n')
                        fscanf(f, "%c", &c);
                    
                    prv = cuddZddGetNode(zdd, d, one, zero);
                    Cudd_Ref(prv);
                    tp = zdd_times(zdd, p, prv);
                    
                    Cudd_RecursiveDerefZdd(zdd, p);
                    Cudd_RecursiveDerefZdd(zdd, prv);
                    p = tp;
                }
                else
                    fscanf(f, "%c", &c);
            }
            while (c != '+' && c != '.' && c != ')' && c != ';' && c != ',');
        }
        else
            fscanf(f, "%c", &c);
        
        tp = zdd_plus(zdd, res, p);
        
        Cudd_RecursiveDerefZdd(zdd, res);
        Cudd_RecursiveDerefZdd(zdd, p);
        
        res = tp;
    }
    
    if (c == '.')
        *p_end = true;
    
    CHECK_NN(res)
    
    return(res);
}



/**
 * read meaningful char from file.
 */
static inline void meaningchar(FILE *f, char *p_c) { //r
    do {
        fscanf(f, "%c", p_c);
    } while ((*p_c) != 'x' && !((*p_c) >= '0' && (*p_c) <= '9') && (*p_c) != '(' && (*p_c) != ')' && (*p_c) != '.' && (*p_c) != ';' && (*p_c) != ',' && (*p_c) != '+');
}



/**
 * recursive function that reads a part each time and returns a single polynomial
 * add reference to the result
 * never return NULL, retuns 0 instead.
 * function terminates when ")" "," ";" "." appear, which is read.
 * seems correct , but not so good.
 * read a poly in a block, i.e. bewteen "+" or "*", or between "(" and ")".
 * needs check.
 */
DdNode *_zdd_readfile(DdManager *zdd, FILE *f, char begin_char, bool *p_end) //r
{
    var_subscript_t d;
    char c;
    DdNode *res, *p, *prv, *tp;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    res = zero;
    Cudd_Ref(res);
    meaningchar(f, &c);
    
    while (c != '.' && c != ')' && c != ';' && c != ',') {
        
        p = one;
        Cudd_Ref(p);
        
        //! return the poly bewteen two "+".
        while (c != '+' && c != '.' && c != ')' && c != ';' && c != ',') {
            if (c == '0') {
                
                Cudd_RecursiveDerefZdd(zdd, p);
                p = zero;
                Cudd_Ref(p);
                meaningchar(f, &c);
                
                CHECK_ASSERT(!(c >= '0' && c <= '9'))
                CHECK_ASSERT(c != 'x')
                
            } else if (c == '1') {
                meaningchar(f, &c);
                
                CHECK_ASSERT(!(c >= '0' && c <= '9'))
                CHECK_ASSERT(c != 'x')
                
            } else if (c == '(') {
                prv = _zdd_readfile(zdd, f, '(', p_end);
                tp = zdd_times(zdd, p, prv);
                Cudd_RecursiveDerefZdd(zdd, prv);
                Cudd_RecursiveDerefZdd(zdd, p);
                p = tp;
                meaningchar(f, &c);
            } else if (c == 'x') {
                meaningchar(f, &c);
                d = 0;
                while (c >= '0' && c <= '9') {
                    d = d * 10 + (c - '0');
                    meaningchar(f, &c);
                }
                
                if (d > ZDD_INPUT_VARNUM) {
                    ERROR("The number of input polynomials exceeds the limit of zdd")
                }
                
                
                prv = cuddZddGetNode(zdd, d, one, zero);
                Cudd_Ref(prv);
                tp = zdd_times(zdd, p, prv);
                Cudd_RecursiveDerefZdd(zdd, p);
                Cudd_RecursiveDerefZdd(zdd, prv);
                p = tp;
            } else
                printf("\nUn admissible input!!!!!\n");
        }
        
        
        tp = zdd_plus(zdd, res, p);
        
        Cudd_RecursiveDerefZdd(zdd, res);
        Cudd_RecursiveDerefZdd(zdd, p);
        
        res = tp;
        
        if (c == '+')
            meaningchar(f, &c);
    }
    
    
    CHECK_ASSERT((begin_char != '(') || (c == ')'))
    
    if (c == '.')
        *p_end = true;
    
    
    
    CHECK_NN(res)
    
    return(res);
}





/**
 * print a single zdd to screen
 * p is not null
 */
void zdd_print(DdManager *zdd, DdNode *p) //r
{
    CHECK_NN(p)
    
    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    if (p == zero)
        printf("0");
    else if (p == one)
        printf("1");
    else {
        t = cuddT(p);
        e = cuddE(p);
        
        printf("x%d",  p->index);
        if (t != one) {
            printf("*");
            if (cuddE(t) == zero) {
                zdd_print(zdd, t);
            } else {
                printf("(");
                zdd_print(zdd, t);
                printf(")");
            }
        }
        
        if (e == zero) {
        } else if (e == one) {
            printf("+1");
        } else {
            printf("+");
            zdd_print(zdd, e);
        }
    }
}




/**
 * print a single zdd with rename to screen.
 * p is not null
 */
void zdd_print(DdManager *zdd, DdNode *p, idrc_t *old_to_new) //r
{
    CHECK_NN(p)
    CHECK_NN(old_to_new)
    
    DdNode *t, *e;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    if (p == zero)
        printf("0");
    else if (p == one)
        printf("1");
    else {
        t = cuddT(p);
        e = cuddE(p);
        
        printf("x%d",  old_to_new[p->index]);
        if (t != one) {
            printf("*");
            if (cuddE(t) == zero) {
                zdd_print(zdd, t, old_to_new);
            } else {
                printf("(");
                zdd_print(zdd, t, old_to_new);
                printf(")");
            }
        }
        
        if (e == zero) {
        } else if (e == one) {
            printf("+1");
        } else {
            printf("+");
            zdd_print(zdd, e, old_to_new);
        }
    }
}




void zdd_markappearance(DdManager *zdd, DdNode *p, bool *vars)
{
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    if (p == one || p == zero)
        return;
    
    vars[p->index] = true;
    
    zdd_markappearance(zdd, cuddT(p), vars);
    zdd_markappearance(zdd, cuddE(p), vars);
}












/**
 * reduce a polynomial p by a list of linear polynomials. return the result of reduction.
 * the result is reffed.
 * p does NOT DISAPPEAR.
 */
DdNode *_zdd_reduce_by_linear(DdManager *zdd, DdNode *p, DdNode **linlist) //r
{
    CHECK_NN(zdd)
    CHECK_NN(p)
    CHECK_NN(linlist)
    
    
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    if (p == one || p == zero) {
        Cudd_Ref(p);
        return p;
    }
    
    
    DdNode *res;

    if (linlist[p->index]) {
        
        CHECK_EQ(cuddT(linlist[p->index]), one)
        
        DdNode *times = zdd_times(zdd, cuddT(p), cuddE(linlist[p->index]));
        DdNode *plus = zdd_plus(zdd, cuddE(p), times);
        
        res = _zdd_reduce_by_linear(zdd, plus, linlist);
        
        Cudd_RecursiveDerefZdd(zdd, times);
        Cudd_RecursiveDerefZdd(zdd, plus);
        
    } else { // p->index cannot be reduced.
        
        DdNode *t = _zdd_reduce_by_linear(zdd, cuddT(p), linlist);
        DdNode *e = _zdd_reduce_by_linear(zdd, cuddE(p), linlist);
        
        res = cuddZddGetNode(zdd, p->index, t, e);
        Cudd_Ref(res);
        
        Cudd_RecursiveDerefZdd(zdd, t);
        Cudd_RecursiveDerefZdd(zdd, e);
    }
    
    return res;
}















// ********************* main functions *********************


/**
 * read file and store in zddpoly_t.
 * zddp is initialized.
 */
void zddpoly_readfile(zddpoly_t *zddp, FILE *in) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    CHECK_NN(in)
    CHECK_EQ(zddp->size_of_polys, 0)
    CHECK_EQ(zddp->rank_of_linear, 0)
    
    
    
    DdManager *zdd = zddp->zdd;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    DdNode *p;
    bool End;
    
    
    // read all polys to zdd_list and linear_zdd
    End = false;
    while (!End) {
        
        CHECK_LT(zddp->rank_of_linear, ZDD_INPUT_VARNUM)
        CHECK_LT(zddp->size_of_polys, ZDD_INPUT_POLYNUM)
        
        p = _zdd_readfile(zdd, in, ' ', &End);
        
        CHECK_NN(p)
        
        if (p == one) {
            
            Cudd_RecursiveDerefZdd(zdd, p);
            
            zddpoly_clear(zddp);
            
            ONEAPPEAR

            
        } else if (p == zero) {
            
            Cudd_RecursiveDerefZdd(zdd, p);
            
        } else {
            
            if (zdd_be_linear(zdd, p)) {// degree one
                zddp->linear[(zddp->rank_of_linear)++] =p;
            } else {
                zddp->polys[(zddp->size_of_polys)++] = p;
            }
        }
    }
    
    
    zddpoly_rename(zddp);
    
}





/**
 * print polynomials from zddpoly structure.
 * flag = 0, only number,  with original names.
 * flag = 1, only numbers, with new names.
 * flag = 2, all polys, with original names.
 * flag = 3, nonlinear polys, with new name.
 * flag = 10, number of nonlinear and all information of linear poly, with original names.
 */
void zddpoly_print(zddpoly_t *zddp, flag_t flag) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    printf("There are %d nonlinear polynomials, and %d(-%d) linear polynomials in %d vars (the max index is %d)\n", zddp->size_of_polys, zddp->rank_of_linear, zddp->deleted_vars, zddp->appeared_vars, zddp->maxindex);
    
        
    if (flag == 2 || flag == 10) {
        if (zddp->size_of_polys) {
            
            if (flag == 2) {
                
                printf("   nonlinear polynomials with original name:\n");
                
                for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
                    zdd_print(zddp->zdd, zddp->polys[i]);
                    
                    if (i == zddp->size_of_polys - 1)
                        printf(".\n");
                    else
                        printf(",\n");
                }
                printf("\n");
            }
            
        } else {
            printf("   there is no nonlinear polynomials.\n\n");
        }
        
        if (zddp->rank_of_linear) {
            printf("   linear polynomials with original name:\n");
            
            for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
                zdd_print(zddp->zdd, zddp->linear[i]);
                
                if (i == zddp->rank_of_linear - 1)
                    printf(".\n");
                else
                    printf(",\n");
            }
            printf("\n");
        } else {
            printf("   there is no linear polynomials.\n\n");
        }
    }
    
    
    
    if (flag == 3) {
        if (zddp->size_of_polys) {
            printf("   nonlinear polynomials with new name:\n");
            
            for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
                zdd_print(zddp->zdd, zddp->polys[i], zddp->old_to_new);

                if (i == zddp->size_of_polys - 1)
                    printf(".\n");
                else
                    printf(",\n");
            }
            printf("\n");
        } else {
            printf("   there is no nonlinear polynomials.\n\n");
        }
        
        if (zddp->rank_of_linear) {
            printf("   linear polynomials with oritinal name:\n");
            
            for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
                zdd_print(zddp->zdd, zddp->linear[i]);

                if (i == zddp->rank_of_linear - 1)
                    printf(".\n");
                else
                    printf(",\n");
            }
            printf("\n");
        } else {
            printf("   there is no linear polynomials.\n\n");
        }
    }

}




bool zdd_linear_constant(zddpoly_t *zddp, DdNode *p){

	DdManager *zdd = zddp->zdd;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    DdNode *e = cuddE(p);

    if(e == one){
    	return 1;
    }else{
    	return 0;
    }

}

/**
 * compute old_to_new and new_to_old for current nonlinear polynomials.
 * this function should be available at any time.
 */
void zddpoly_rename(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    
    if (zddp->size_of_polys == 0) {
        zddp->appeared_vars = 0;
        return;
    }
    
    
    
    // find appearing variables.
    bool *vars = (bool *) gb_calloc(ZDD_INPUT_VARNUM, sizeof(bool));
    CHECK_MEM(vars);
    
    for (idrc_t i = 0; i < zddp->size_of_polys; i++)
        zdd_markappearance(zddp->zdd, zddp->polys[i], vars);
    
    //ioooooo
    printf("\n necessary variables:\n");
    zddp->appeared_vars = 0;
    
    for (var_subscript_t i = 0; i < ZDD_INPUT_VARNUM; i++) {
        
        if (vars[i]) {
            
            printf("   x%d(%d)", i,zddp->appeared_vars);
            
            zddp->old_to_new[i] = zddp->appeared_vars;
            zddp->new_to_old[(zddp->appeared_vars)++] = i;
            
            zddp->maxindex = i;
            
        }
    }
    
    gb_free(vars);
    
}



/**
 * add a guessed var to zddpoly.
 * val == 0, x_ind
 * val == 1, x_ind + 1.
 */
void zddpoly_guessvar(zddpoly_t *zddp, var_subscript_t ind, char val) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    CHECK_GE(ind, 0);
    CHECK_LT(ind, ZDD_INPUT_VARNUM)
    CHECK_ASSERT(val == 0 || val == 1)
    
    
    DdManager *zdd = zddp->zdd;
    DdNode *p;
    
    if (val == 0) {
        DdNode *one = DD_ONE(zdd);
        DdNode *zero = DD_ZERO(zdd);
        p = cuddZddGetNode(zdd, ind, one, zero);
        Cudd_Ref(p);
    } else {
        DdNode *one = DD_ONE(zdd);
        p = cuddZddGetNode(zdd, ind, one, one);
        Cudd_Ref(p);
    }
    
    
    zddp->linear[(zddp->rank_of_linear)++] = p;
    
}





/**
 * read a line a matrix and stored as a linear zdd.
 */
void zddpoly_guesslinear(zddpoly_t *zddp, var_subscript_t *inds, idrc_t length, char val) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    CHECK_ASSERT(val == 0 || val == 1)
    
    
    
    DdManager *zdd = zddp->zdd;
    DdNode *p;
    DdNode *one = DD_ONE(zdd);
    DdNode *zero = DD_ZERO(zdd);
    
    
    if (val == 0) {
        p = zero;
        Cudd_Ref(p);
    } else {
        p = one;
        Cudd_Ref(p);
    }
    
    
    DdNode *tp, *plus;
    for (idrc_t i = 0; i < length; i++) {
        
        CHECK_GE(inds[i], 0);
        CHECK_LT(inds[i], ZDD_INPUT_VARNUM)
        
        tp = cuddZddGetNode(zdd, inds[i], one, zero);
        Cudd_Ref(tp);
        plus = zdd_plus(zdd, p, tp);
        Cudd_RecursiveDerefZdd(zdd, p);
        Cudd_RecursiveDerefZdd(zdd, tp);
        p = plus;
    }
    
    zddp->linear[(zddp->rank_of_linear)++] = p;
}







/**
 * interreduce linear polynomials in zddpoly.
 * polynomials duplicated leading monomials are put into polys, so all leading monomails of linear are different.
 */
void zdd_interreduce_linear(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    
    if (zddp->rank_of_linear == 0)
        return;
    
    
    // use a list of linear polynomials.
    DdNode **linlist = (DdNode **)gb_calloc(ZDD_INPUT_VARNUM, sizeof(DdNode *));
    CHECK_MEM(linlist)
    
    
    // put all linear polynomails into linlist.
    // pick out duplicated leading monomials.
    
    
    for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
        
        if (linlist[zddp->linear[i]->index]) {
            zddp->polys[(zddp->size_of_polys)++] = zddp->linear[i];
        } else {
            linlist[zddp->linear[i]->index] = zddp->linear[i];
        }
        
        zddp->linear[i] = NULL;
    }
    
    zddp->rank_of_linear = 0;
    
    
    
    
    // reduce in linlist.
    for (var_subscript_t i = 0; i < ZDD_INPUT_VARNUM; i++) {
        
        if (linlist[i] == NULL)
            continue;
        
        CHECK_NE(linlist[i], DD_ZERO(zddp->zdd))
        CHECK_NE(linlist[i], DD_ONE(zddp->zdd))
        CHECK_EQ(linlist[i]->index, i)
        CHECK_EQ(cuddT(linlist[i]), DD_ONE(zddp->zdd))
        
        DdNode *e = _zdd_reduce_by_linear(zddp->zdd, cuddE(linlist[i]), linlist);
        zddp->linear[(zddp->rank_of_linear)] = cuddZddGetNode(zddp->zdd, i, DD_ONE(zddp->zdd), e);
        Cudd_Ref(zddp->linear[(zddp->rank_of_linear)]);
        (zddp->rank_of_linear)++;
        
        Cudd_RecursiveDerefZdd(zddp->zdd, e);
        Cudd_RecursiveDerefZdd(zddp->zdd, linlist[i]);
        
        linlist[i] = NULL;
    }
    
    gb_free(linlist);
}



/**
 * reduce nonlinear polys by linear polynomials in zddpoly.
 */
void zdd_nonlinear_reduceby_linear(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    
    
    //if (zddp->deleted_vars == zddp->rank_of_linear)
       // return;
    
    
    
    // use a list of linear polynomials.
    DdNode **linlist = (DdNode **)gb_calloc(ZDD_INPUT_VARNUM, sizeof(DdNode *));
    CHECK_MEM(linlist)
    
    
    // put all linear polynomails into linlist.
    for (var_subscript_t i = 0; i < zddp->rank_of_linear; i++) {
        
        CHECK_EQ(linlist[zddp->linear[i]->index], NULL)
        linlist[zddp->linear[i]->index] = zddp->linear[i];
    } // no extra refs
    
    zddp->deleted_vars = zddp->rank_of_linear;
    
    
    
    // reduce polys.
    // store in place.
    DdNode *one = DD_ONE(zddp->zdd);
    DdNode *zero = DD_ZERO(zddp->zdd);
    DdNode *p;
    idrc_t newpos = 0;

    
    for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
        
        CHECK_NN(zddp->polys[i])
        CHECK_NE(zddp->polys[i], zero)
        CHECK_NE(zddp->polys[i], one)
        
        p = _zdd_reduce_by_linear(zddp->zdd, zddp->polys[i], linlist);
        
        if (p == one) {
            
            Cudd_RecursiveDerefZdd(zddp->zdd, p);
            zddpoly_clear(zddp);
            gb_free(linlist);
            
            ERROR("Input polynomials reduced to 1")

        } else if (p == zero) {
            
            Cudd_RecursiveDerefZdd(zddp->zdd, p);
            Cudd_RecursiveDerefZdd(zddp->zdd, zddp->polys[i]);
            zddp->polys[i] = NULL;
            
        } else {
            Cudd_RecursiveDerefZdd(zddp->zdd, zddp->polys[i]);
            zddp->polys[i] = NULL;
            
            if (zdd_be_linear(zddp->zdd, p)) {// degree one
                zddp->linear[(zddp->rank_of_linear)++] =p;
            } else {
                zddp->polys[newpos++] = p;
            }
        }
    }
    zddp->size_of_polys = newpos;
    
    gb_free(linlist);
}








/**
 * reduce nonlinear polyomials by linear ones.
 * since deleted vars have been recorded, only need to reduce new generated ones.
 */
void zddpoly_simplify(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    CHECK_LE(zddp->deleted_vars, zddp->rank_of_linear)
    
    
    //printf("\n  Befor simplification, there are %d nonlinear polys in %d vars, and %d(-%d) linear polynomials.\n", zddp->size_of_polys, zddp->appeared_vars, zddp->rank_of_linear, zddp->deleted_vars);

    while (zddp->deleted_vars < zddp->rank_of_linear) {
        
        // inter reduce linear polynomials.
        zdd_interreduce_linear(zddp);
        
        //zddpoly_print(zddp, 2);
        
        //printf("\n  %d new linear polynomials are found during reduction\n", zddp->rank_of_linear - zddp->deleted_vars);
        

        zdd_nonlinear_reduceby_linear(zddp);
        
        //zddpoly_print(zddp, 0);
    }
    
    
    zddpoly_rename(zddp);
    
    printf("\n  After simplification, there are %d nonlinear polys in %d vars, and %d(-%d) linear polynomials.\n\n", zddp->size_of_polys, zddp->appeared_vars, zddp->rank_of_linear, zddp->deleted_vars);
    



    
    
}





/**
 * return the maximal degree of nonlinear polynomials.
 */
deg_t zddpoly_maxdegree(zddpoly_t *zddp) //r
{
#if DEBUG_MODE
    zddpoly_check(zddp);
#endif
    
    deg_t mdeg = 0, d;
    for (idrc_t i = 0; i < zddp->size_of_polys; i++) {
        
        d = zdd_degree(zddp->zdd, zddp->polys[i]);
        if (mdeg < d) {
            mdeg = d;
        }
    }
    
    return mdeg;
}
























