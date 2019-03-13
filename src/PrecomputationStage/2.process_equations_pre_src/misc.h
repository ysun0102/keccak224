#include "config.h"

//! macro for debug

#define PR_INT(a) {printf("%d\n", a);}
#define PR_LONG(a) {printf("%ld\n", a);}

#define PR1 {printf("****************1\n");}
#define PR2 {printf("============2\n");}
#define PR3 {printf("&&&&&&&&&&&&&3\n");}
#define PR4 {printf("@@@@@@@@@@@@4\n");}
#define PR5 {printf("#############5\n");}
#define PR6 {printf("^^^^^^^^^^^^^^6\n");}

#define PR_MON_ID(n) {printf("mon: "); mon_print(IDNode(n).deg, IDNode(n).subscript); printf("\n");}



#if DEBUG_MODE

#define CHECK_NN(p) {if (p == NULL) { printf("Found null pointer!\n"); abort(); }}

#define CHECK_GE(a, b) {if (a < b) {printf("GE does not hold!\n"); abort();}}
#define CHECK_GT(a, b) {if (a <= b) {printf("GT does not hold!\n"); abort();}}

#define CHECK_LE(a, b) {if (a > b) {printf("LE does not hold!\n"); abort();}}
#define CHECK_LT(a, b) {if (a >= b) {printf("LT does not hold!\n"); abort();}}

#define CHECK_EQ(a, b) {if (a != b) {printf("EQ does not hold!\n"); abort();}}
#define CHECK_NE(a, b) {if (a == b) {printf("NE does not hold!\n"); abort();}}


#define CHECK_MEM(p) {if (p == NULL) { printf("run out of memory!!\n"); abort(); }}

#define CHECK_ASSERT(p) {if (!(p)) {printf("assert fail!\n"); abort();} }

#define CHECK_DEGREE(d) {if (!(d >= 0 && d < DEGREE_LIMIT)) {printf("degree is not available!\n"); abort(); }}
#define CHECK_SUBS(s) {if (!(s >= 0 && s < VAR_LIMIT)) {printf("subscript is not available!\n"); abort(); }}
#define CHECK_MINDEX(s) {if (!(s >= 0 && s < MATRIX_LIMIT)) {printf("matrix index  is not available!\n"); abort(); }}


#define CHECK_MAT_ROWPOS(M, rowpos) {if (!(rowpos >= 0 && rowpos < M->nrows)) {printf("row position is not available!\n"); abort(); }}

#define CHECK_MAT_COLPOS(M, colpos) {if (!(colpos >= 0 && colpos < M->ncols)) {printf("col position is not available!\n"); abort(); }}

#else


#define CHECK_NN(p) {}

#define CHECK_GE(a, b) {}
#define CHECK_GT(a, b) {}

#define CHECK_LE(a, b) {}
#define CHECK_LT(a, b) {}

#define CHECK_EQ(a, b) {}
#define CHECK_NE(a, b) {}


#define CHECK_MEM(p) {}

#define CHECK_ASSERT(p) {}

#define CHECK_DEGREE(d) {}
#define CHECK_SUBS(s) {}
#define CHECK_MINDEX(s) {}

#define CHECK_MAT_ROWPOS(M, rowpos) {}
#define CHECK_MAT_COLPOS(M, colpos) {}

#endif


//! for memory

static inline void *gb_malloc(size_t n, size_t size) { //r
    CHECK_GE(n, 0)
    CHECK_GE(size, 0)
    
    return (malloc(GB_MAX(n, 1) * size));
}


static inline void *gb_calloc(size_t n, size_t size) { //r
    CHECK_GE(n, 0)
    CHECK_GE(size, 0)
    
    return (calloc(GB_MAX(n, 1),  size));
}


static inline void * gb_realloc(void *p, size_t n, size_t size) { //r
    CHECK_GE(n, 0);
    CHECK_GE(size, 0)
    
    return (realloc(p, GB_MAX(n, 1) * size)); //!!!!!!!!!!!!!!!!!! (char *)?
}


static inline void *gb_free(void *p) { //r
    if (p)
        free(p);
    return (NULL);
}


