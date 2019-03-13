
// ********************* inlcudes ************************

// standard lib
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
//#include <sys/malloc.h>


#include <stdio.h>
#include <unistd.h>


// cudd
#include <util.h>
#include <cudd.h>
#include <cuddInt.h>



// ! type redefinition
typedef unsigned long gb_word_t;
typedef unsigned short var_subscript_t; //! subscripts of xi. 0 <= i < 2^16
typedef char deg_t; //! degree
typedef int idrc_t; //! size of rows and cols in a matrix, or size of monomials.
typedef char flag_t; //! flags or status
typedef long count_t;
//typedef unsigned long hashvalue_t;

typedef long value_t; // to save values of variables.







//! useful functions
#define GB_MAX(a, b) (a > b ? a : b)
#define GB_MIN(a, b) (a < b ? a : b)

#define INFO_PR(s) {printf("\n************ %s *************\n\n", s);}

#define ERROR(s) {printf("\n ERROR !!!!!!------------- %s ----------------\n\n", s); exit(1);}

#define ONEAPPEAR {printf("\n+++++++ 1 appears +++++++\n\n"); return;}

#define PR_ENTER {printf("\n");}





