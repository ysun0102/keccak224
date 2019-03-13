# A practical preimage attack on 3-round Keccak-224

Related codes for the paper "Preimage Attacks on Round-reduced Keccak-224/256 via an Allocating Approach" (https://eprint.iacr.org/2019/248) by Ting Li(liting@iie.ac.cn) and Yao Sun(http://people.ucas.ac.cn/~yaosun, sunyao@iie.ac.cn). 


Structures
-----------------------------------------------------------
    .
    ├── run                                 
        ├── PrecomputationStage
              ├── exe
              └── data
                    ├── equ_files
                    ├── mat_files
                    └── state_files
        ├── OnlineStage
              ├── exe
              └── data
                    ├── equ_files
                    ├── mat_files
                    └── state_files
        └── Verify
    ├── src
        ├── PrecomputationStage
              ├── generate_equations_pre_src
              ├── process_equations_pre_src
              ├── solve_equations_pre_src
              └── parse_result_pre_src
        ├── OnlineStage
              ├── hash_inv_src
              ├── generate_equations_online_src
              ├── process_equations_online_src
              ├── solve_equations_online_src
              └── parse_result_online_src
        └── Verify
    └── README.md






Procedure of the attack
-----------------------------------------------------------

Precompuatation Stage：
1. Generate an algebraic system from the setting in the paper
2. Convert polynomials to matrices for speeding up computations
3. Solving this algebraic system using GPU
4. Interpret the found solution

Online Stage：
1. Generate 192 linear relations via the inverse of Chi for a given digest
2. Convert the above linear equations to an algebraic system
3. Convert polynomials to matrices for speeding up computations
4. Solving this algebraic system using GPU
5. Interpret the found solution, and output a preimage for the given digest


Compilation:
-----------------------------------------------------------

To complie the codes, the package CUDD must be installed. One can download this package from http://www.davidkebo.com/source/cudd_versions/cudd-3.0.0.tar.gz, and then install it by the following cammand.

```
./configure --enable-dddmp --enable-obj --enable-shared --enable-static ; make
```

The codes can be complied in the following way.

```
gcc test.c -0 testprogram -I $(CUDD_PATH)/cudd -I $(CUDD_PATH)/util -I $(CUDD_PATH)/dddmp -I $(CUDD_PATH)/mtr -I $(CUDD_PATH)/epd -I $(CUDD_PATH)/st -lpthread -L $(CUDD_PATH)/cudd/.libs/ -lcudd -lm
```
