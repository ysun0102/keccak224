#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>

//#include <helper_cuda.h>

#define DEBUG 0
#define ENUM_NUM 17 //16


#define UNKNOWN_NUM 64
#define POLY_NUM 64
#define PARA_NUM 64
#define NONLINEAR_NUM 65


#define SOL_MAX_NUM 200
#define RESULT_MAX_NUM 5

//#define SET_VAL (((value_t)14764175488)<< ENUM_NUM)

#define BLOCK_NUM 32 //2^5
#define THREAD_NUM  256 // 2^8
#define THREADS_SHIFT 13 // (5+8)
typedef long value_t; // to save values of variables.

//typedef unsigned long constpart_t; // the part with no parameters.
typedef unsigned long linearpart_t; // to save 32 unknowns and 1 contants.
typedef unsigned long squarepart_t;

typedef unsigned long oripoly_t;



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


typedef unsigned char UINT8;
typedef unsigned long long UINT64;


__device__ linearpart_t d_linear_mat[ENUM_NUM * POLY_NUM * 2];
__device__ oripoly_t d_polys_mat[NONLINEAR_NUM * (POLY_NUM + UNKNOWN_NUM + 1) * 3];
__device__ squarepart_t d_square_mat[ENUM_NUM * POLY_NUM];
__device__ value_t d_var_all[2560];







static inline __host__ __device__ int largestpos(value_t val, int len) {

	for (int i = len - 1; i >= 0; i--) {
		if (val & ((value_t) 1 << i)) {
			return i;
		}
	}

	return -1;
}


static inline __host__ __device__ int largestpos_2(value_t val0, value_t val1, int len) {
	int p = 0;
	if (len > 64 && len <= 128) {
		p = largestpos(val1, len - 64);
		if (p > -1) {
			return p + 64;
		} else {
			p = largestpos(val0, 64);

		}
	} else {
		p = largestpos(val0, 64);

	}

	return p;
}


static inline value_t gauss_host(linearpart_t working_mat[POLY_NUM][2],
		const int poly_num, const int unknown_num, value_t solutions[SOL_MAX_NUM]) {

	int pos_arr[POLY_NUM]; // bear revised
	int rank = 0;

	for (int pi = 0; pi < POLY_NUM; pi++) {

		if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
			continue;
		}


		pos_arr[pi] = largestpos_2(working_mat[pi][0],working_mat[pi][1], unknown_num + 1);

		rank++;
		if (pos_arr[pi] == 0) {
			return 0;
		}





		for (int j = pi + 1; j < POLY_NUM; j++) {

			if(working_mat[j][pos_arr[pi]/64] & ((linearpart_t)1 << (pos_arr[pi] % 64))){
                working_mat[j][0] ^= (working_mat[pi][0]);
                working_mat[j][1] ^= (working_mat[pi][1]);
            }
		}



	}


	// back
	for (int pi = 0; pi < POLY_NUM; pi++) {

		if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
			continue;
		}

		for (int j = 0; j < pi; j++) {
			if (working_mat[j][pos_arr[pi] / 64]
					& ((linearpart_t) 1 << (pos_arr[pi] % 64))) {
				working_mat[j][0] ^= (working_mat[pi][0]);
				working_mat[j][1] ^= (working_mat[pi][1]);
			}
		}
	}

	if (rank == unknown_num) {

		// only one solution.
		solutions[0] = 0;
;
		for (int pi = 0; pi < POLY_NUM; pi++) {

			if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0 ) {
				continue;
			}
			if (working_mat[pi][0] & (linearpart_t)1) {
				solutions[0] ^= ((value_t)1 << (pos_arr[pi]-1));
			}
		}

		return 1;

	} else {

		// now troubles come
		solutions[0] = 0;
		value_t sol_num = 1;
		bool appear[UNKNOWN_NUM + 1] = { 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



		for (int pi = 0; pi < POLY_NUM; pi++) {

			if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
				continue;
			}

			appear[pos_arr[pi]] = true;
			if (working_mat[pi][0] & (linearpart_t)1) {
				solutions[0] ^= ((value_t)1 << (pos_arr[pi]-1));
			}
		}

		// duplicate solutions.
		for (int i = 1; i < UNKNOWN_NUM+1; i++) {  // liting revised

			if (appear[i] == false) {


				for (int j = 0; j < sol_num; j++) {
				    // bear revised
                    solutions[j + sol_num] = (solutions[j]) ^ ((value_t)1 << (i-1));
				}

				// bear added
                for (int pi = 0; pi < POLY_NUM; pi++) {
				    if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0 ) {
			            continue;
		            }

					if(i < 64){
						for (int j = 0; j < sol_num * ((working_mat[pi][0] & (((linearpart_t) 1) << i)) != 0); j++) {

			    		            solutions[j + sol_num] ^= ((value_t) 1 << (pos_arr[pi] - 1));
			    		   	}
					}else{
						for (int j = 0; j < sol_num * ((working_mat[pi][1] & (((linearpart_t) 1) << i-64)) != 0); j++) {

			    		            solutions[j + sol_num] ^= ((value_t) 1 << (pos_arr[pi] - 1));
			    		   	}
					}


				}


				sol_num *= 2;

			}
		}

		return sol_num;

	}

}




static inline __device__ value_t gauss(value_t solutions[SOL_MAX_NUM], linearpart_t working_mat[POLY_NUM][2],
		const int poly_num, const int unknown_num) {

	// bear revised
	int pos_arr[POLY_NUM]; // bear revised
	int rank = 0;

	for (int pi = 0; pi < POLY_NUM; pi++) {

		if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
			continue;
		}

		pos_arr[pi] = largestpos_2(working_mat[pi][0], working_mat[pi][1],unknown_num + 1);
		rank++;


		if (pos_arr[pi] == 0) {
			return 0;
		}

		for (int j = pi + 1; j < POLY_NUM; j++) {

			if (working_mat[j][pos_arr[pi] / 64]
					& ((linearpart_t) 1 << (pos_arr[pi] % 64))) {
				working_mat[j][0] ^= (working_mat[pi][0]);
				working_mat[j][1] ^= (working_mat[pi][1]);
			}
		}

	}

	// back
	for (int pi = 0; pi < POLY_NUM; pi++) {

		if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
			continue;
		}

		for (int j = 0; j < pi; j++) {
			if (working_mat[j][pos_arr[pi] / 64]
					& ((linearpart_t) 1 << (pos_arr[pi] % 64))) {
				working_mat[j][0] ^= (working_mat[pi][0]);
				working_mat[j][1] ^= (working_mat[pi][1]);
			}
		}
	}

	if (rank == unknown_num) {

		// only one solution.
		solutions[0]= 0;
		for (int pi = 0; pi < POLY_NUM; pi++) {

			if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
				continue;
			}
			if (working_mat[pi][0] & (linearpart_t) 1) {
				solutions[0] ^= ((value_t)1 << (pos_arr[pi]-1));
			}
		}

		return 1;

	} else {

		// now troubles come
		// now troubles come
		solutions[0] = 0;
		value_t sol_num = 1;
		//liting revised
		bool appear[UNKNOWN_NUM + 1] = { 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		for (int pi = 0; pi < POLY_NUM; pi++) {

			if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0) {
				continue;
			}

			appear[pos_arr[pi]] = true;
			if (working_mat[pi][0] & (linearpart_t) 1) {
				solutions[0] ^= ((value_t)1 << (pos_arr[pi]-1));
			}
		}

		// duplicate solutions.
		for (int i = 1; i < UNKNOWN_NUM + 1; i++) {  // liting revised

			if (appear[i] == false) {

				for (int j = 0; j < sol_num; j++) {
					// bear revised
				    solutions[j + sol_num] = (solutions[j]) ^ ((value_t)1 << (i-1));
				}

				// bear added
				for (int pi = 0; pi < POLY_NUM; pi++) {
					if (working_mat[pi][0] == 0 && working_mat[pi][1] == 0 ) {
						continue;
					}

					if(i < 64){
						for (int j = 0; j < sol_num * ((working_mat[pi][0] & (((linearpart_t) 1) << i)) != 0); j++) {

							solutions[j + sol_num] ^= ((value_t) 1 << (pos_arr[pi] - 1));
						}
					}else{
						for (int j = 0; j < sol_num * ((working_mat[pi][1] & (((linearpart_t) 1) << i-64)) != 0); j++) {

							 solutions[j + sol_num] ^= ((value_t) 1 << (pos_arr[pi] - 1));
						}
					}


				}
				sol_num *= 2;

			}
		}

		return sol_num;

	}

}





__global__ void solveLinear(const linearpart_t *d_working_mat_copy,
		const squarepart_t *d_const_mat, value_t *d_val, char *d_bound, value_t *d_sol_total,value_t* result) {

	int thidx = blockDim.x * blockIdx.x + threadIdx.x;
	value_t val = d_val[thidx];
	char bound = d_bound[thidx];
	value_t res[2 * RESULT_MAX_NUM];
	char res_num = 0;
	linearpart_t working_mat[POLY_NUM][2]; // initialized as the const part of linear matrix. also used as the results of linear part.
	linearpart_t working_mat_copy[POLY_NUM][2];

	squarepart_t const_mat[POLY_NUM];
	d_sol_total[thidx] = 0;
	oripoly_t cstpoly[3] = {0, 0, 0};
	//copy data from device
	for(int i = 0; i < 2 * RESULT_MAX_NUM; i++){
		res[i] = 0;
	}
	res[0] = result[thidx * 2 * RESULT_MAX_NUM];
	res[1] = result[thidx * 2 * RESULT_MAX_NUM + 1];


	for (int i = 0; i < POLY_NUM; i++) {
		working_mat_copy[i][0] = d_working_mat_copy[thidx * POLY_NUM * 2 + i*2];
		working_mat_copy[i][1] = d_working_mat_copy[thidx * POLY_NUM * 2 + i*2 + 1];

		const_mat[i] = d_const_mat[thidx * POLY_NUM + i];
	}

	//todo to be finished!!!!
//	for(int i=0; i< NONLINEAR_NUM;i++){
//		for(int j = 0; j < PARA_NUM + UNKNOWN_NUM + 1; j++){
//			polys[i][j][0] = d_polys_mat[i * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + j * 3];
//			polys[i][j][1] = d_polys_mat[i * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + j * 3 + 1];
//			polys[i][j][2] = d_polys_mat[i * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + j * 3 + 2];
//
//		}
//	}

	// main loop.
	for (value_t count = 1; count < (1 << ENUM_NUM); count++) {

		// generate the next gray code
		int pos = 64-__ffsll(__brevll(count ^ (count - 1)));

		val = val ^ ((value_t) 1 << pos);


		for (int pi = 0; pi < POLY_NUM; pi++) {
			working_mat_copy[pi][0] ^= d_linear_mat[pos * POLY_NUM * 2 + pi * 2];
			working_mat_copy[pi][1] ^= d_linear_mat[pos * POLY_NUM * 2 + pi * 2 + 1];
			const_mat[pi] ^= d_square_mat[pos * POLY_NUM + pi];

			working_mat[pi][0] = working_mat_copy[pi][0];
			working_mat[pi][1] = working_mat_copy[pi][1];


			value_t w = const_mat[pi] & val;


			working_mat[pi][0] ^= (bool)((__popcll((unsigned long long int)w)) & (value_t) 1);


		}




		value_t solutions[SOL_MAX_NUM];
		value_t sol_num = 0;


		// gauss
		sol_num = gauss(solutions, working_mat, POLY_NUM, UNKNOWN_NUM);
		d_sol_total[thidx] += sol_num;
		// verify on 3 round keccak.

		for(int s = 0;s < sol_num;s++){
			int zero_num = 0;
			int one_num = 0;
			int op;

			for (op = 0; op < NONLINEAR_NUM; op++) {

				cstpoly[0] = d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + UNKNOWN_NUM) * 3];
				cstpoly[1] = d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + UNKNOWN_NUM) * 3 + 1];
				cstpoly[2] = d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + UNKNOWN_NUM) * 3 + 2];

//					cstpoly[0] = polys[op][PARA_NUM + UNKNOWN_NUM][0];
//					cstpoly[1] = polys[op][PARA_NUM + UNKNOWN_NUM][1];
//					cstpoly[2] = polys[op][PARA_NUM + UNKNOWN_NUM][2];
				 	// for parameters.
					for (int pa = 0; pa < PARA_NUM; pa++) {
						if (val & ((value_t) 1 << pa)) {
							cstpoly[0] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM - pa -1) * 3];
							cstpoly[1] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM - pa -1) * 3 + 1];
							cstpoly[2] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM - pa -1) * 3 + 2];

//							cstpoly[0] ^= polys[op][PARA_NUM - pa -1][0];
//							cstpoly[1] ^= polys[op][PARA_NUM - pa -1][1];
//							cstpoly[2] ^= polys[op][PARA_NUM - pa -1][2];
						}
					}

					for (int un = 0; un < UNKNOWN_NUM; un++) {
						if (solutions[s] & ((value_t) 1 << un)) {
							cstpoly[0] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + un) * 3];
							cstpoly[1] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + un) * 3 + 1];
							cstpoly[2] ^= d_polys_mat[op * (PARA_NUM + UNKNOWN_NUM + 1) * 3 + (PARA_NUM + un) * 3 + 2];
//							cstpoly[0] ^= polys[op][ PARA_NUM + un][0];
//							cstpoly[1] ^= polys[op][ PARA_NUM + un][1];
//							cstpoly[2] ^= polys[op][ PARA_NUM + un][2];
						}
					}


					// evaluate
					cstpoly[0] = cstpoly[0] & val;
					cstpoly[1] = cstpoly[1] & solutions[s];
					cstpoly[2] = cstpoly[2] & ((oripoly_t)1);
					value_t w = cstpoly[0] ^ cstpoly[1] ^ cstpoly[2];

					w = (w) ^ (w >> 32);
					w = (w) ^ (w >> 16);
					w = (w) ^ (w >> 8);
					w = (w) ^ (w >> 4);
					w = (w) ^ (w >> 2);
					w = (w) ^ (w >> 1);


					if (w & (value_t) 1) {

					    zero_num ++;
					    if(zero_num > NONLINEAR_NUM - bound){
						break;
					    }
					}else{
					    one_num++;

					}


				}

			if(zero_num + one_num == NONLINEAR_NUM && one_num >= bound){
				if(one_num > bound){
					bound = one_num;
					res_num = 0;
					for(int ir = 0; ir < 2 * RESULT_MAX_NUM; ir++ ){
						res[ir] = 0;
					}
				}
				if(res_num < RESULT_MAX_NUM){
					res[res_num * 2] = val;
					res[res_num * 2 + 1 ] = solutions[s];
					res_num ++;
				}
//				    printf("\nval:%lu,sol:%lu,count:%d(bound:%d) thidx:%d  \n",val,solutions[s],one_num,bound, thidx);


			}

		}
	}

	d_bound[thidx] = bound;
	for(int i = 0; i < 2 * RESULT_MAX_NUM; i++){
		result[thidx * 2 * RESULT_MAX_NUM + i] = res[i];
	}

}






int main(int argc, char** argv) {


	char bound = 0;
	const int para_num = PARA_NUM;
	const int enum_num = ENUM_NUM;
	const int ori_num = NONLINEAR_NUM;
	//const int set_num = para_num - enum_num;

	value_t set_val = atol(argv[1])<<THREADS_SHIFT;
//	value_t set_val = 0;

	const int poly_num = POLY_NUM;
	const int unknown_num = UNKNOWN_NUM;

	linearpart_t linear_mat[para_num][poly_num][2];
	linearpart_t working_mat[poly_num][2]; // initialized as the const part of linear matrix. also used as the results of linear part.
	linearpart_t working_mat_copy[poly_num][2];
	linearpart_t working_mat_file[poly_num][2];

	squarepart_t square_mat[para_num][poly_num];
	squarepart_t const_mat[poly_num]; // used to compute the const part from square polys.

    oripoly_t polys[ori_num][para_num + unknown_num + 1][3];
    oripoly_t cstpoly[3];
//	cudaSetDevice(0);
	cudaSetDevice(atoi(argv[2]));

	FILE *in1 = fopen("../data/mat_files/linear_mat.txt", "r+");
	FILE *in2 = fopen("../data/mat_files/square_mat.txt", "r+");
	FILE *in3 = fopen("../data/mat_files/poly_mat.txt", "r+");
	FILE *in4 = fopen("../data/mat_files/working_mat.txt", "r+");

	FILE *out = fopen("../data/mat_files/solve_mat_result.txt","a+");

	char c1, c2, c3, c4;
	for (int i = 0; i < para_num; i++) {
		for (int j = 0; j < poly_num; j++) {
			linear_mat[i][j][0] = 0;
			linear_mat[i][j][1] = 0;
			square_mat[i][j] = 0;

			for (int k = 0; k < 128; k++) {
				fscanf(in1, "%c", &c1);
				while (c1 != '0' && c1 != '1') {
					fscanf(in1, "%c", &c1);
				}
				if (c1 == '1') {

					linear_mat[i][j][k/64] ^= ((linearpart_t) 1 << (k-((int)k/64)*64));
				}
			}
			for (int k = 0; k < para_num; k++) {
				fscanf(in2, "%c", &c2);
				while (c2 != '0' && c2 != '1') {
					fscanf(in2, "%c", &c2);
				}
				if (c2 == '1') {
					square_mat[i][j] ^= ((squarepart_t) 1 << (para_num - 1) - k);
				}
			}
		}
	}



	for (int i = 0; i < ori_num; i++) {
		for (int j = 0; j < para_num + unknown_num + 1; j++) {
			polys[i][j][0] = 0;
			polys[i][j][1] = 0;
			polys[i][j][2] = 0;
			for (int k = 0; k < 192; k++) {
				fscanf(in3, "%c", &c3);
				while (c3 != '0' && c3 != '1') {
					fscanf(in3, "%c", &c3);
				}
				if (k < para_num && c3 == '1') {
					polys[i][j][0] ^= ((oripoly_t) 1 << (para_num - k -1));
				} else if (k >= para_num && k<para_num+unknown_num && c3 == '1') {
					polys[i][j][1] ^= ((oripoly_t) 1 << (k - para_num));
				}else if(c3 == '1'){
					polys[i][j][2] ^= ((oripoly_t) 1);
				}
			}

		}

	}


	for (int i = 0; i < poly_num; i++) {
		working_mat[i][0] = 0;
		working_mat[i][1] = 0;
		for (int j = 0; j < 128; j++) {
			fscanf(in4, "%c", &c4);
			while (c4 != '0' && c4 != '1') {
				fscanf(in4, "%c", &c4);
			}
			if (c4 == '1') {

				working_mat[i][(int)j/64] ^= ((linearpart_t) 1 << (j - ((int)j/64)*64));
			}
		}

		working_mat_file[i][0] = working_mat[i][0];
		working_mat_file[i][1] = working_mat[i][1];


	}

	fclose(in1);
	fclose(in2);
	fclose(in3);
	fclose(in4);


	printf("finish reading file!\n");


	linearpart_t linear_mat_enum[ENUM_NUM * POLY_NUM * 2];
	squarepart_t square_mat_enum[ENUM_NUM * POLY_NUM];
	oripoly_t polys_mat_enum[ori_num * (para_num + unknown_num + 1) * 3 ];

	for (int i = 0; i < ENUM_NUM; i++) {
		for (int j = 0; j < POLY_NUM; j++) {
			for (int k = 0; k < 2; k++) {
				linear_mat_enum[i * POLY_NUM * 2 + j * 2 + k] =
						linear_mat[i][j][k];

			}
		}
	}

	for (int i = 0; i < ENUM_NUM; i++) {
		for (int j = 0; j < POLY_NUM; j++) {
			square_mat_enum[i * POLY_NUM + j] = square_mat[i][j];
		}
	}

	for(int i = 0; i < ori_num; i++){
		for(int j = 0; j < para_num + unknown_num + 1; j++){
			for(int k = 0; k < 3; k++){
				polys_mat_enum[i * (para_num + unknown_num + 1) * 3 + j * 3 + k] = polys[i][j][k];
			}
		}
	}

	cudaMemcpyToSymbol(d_linear_mat, linear_mat_enum,
			2 * ENUM_NUM * POLY_NUM * sizeof(linearpart_t));
	cudaMemcpyToSymbol(d_square_mat, square_mat_enum,
			ENUM_NUM * POLY_NUM * sizeof(squarepart_t));
	cudaMemcpyToSymbol(d_polys_mat, polys_mat_enum,
				3 * NONLINEAR_NUM * (PARA_NUM + UNKNOWN_NUM + 1)  * sizeof(oripoly_t));

	printf("finish copying device memory!\n");

	cudaError_t err = cudaSuccess;
	int thidx = BLOCK_NUM * THREAD_NUM;

	value_t *d_val = NULL;
	err = cudaMalloc((void **) &d_val, thidx * sizeof(value_t));
	if (err != cudaSuccess) {
		printf("Failed to allocate device value (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}


	char *d_bound = NULL;
		err = cudaMalloc((void **) &d_bound, thidx * sizeof(char));
		if (err != cudaSuccess) {
			printf("Failed to allocate device value (error code %s)!\n",
					cudaGetErrorString(err));
			exit(EXIT_FAILURE);
		}

	value_t *d_sol_total = NULL;
	err = cudaMalloc((void **) &d_sol_total, thidx * sizeof(value_t));
	if (err != cudaSuccess) {
		printf("Failed to allocate device value (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	linearpart_t *d_working_mat_copy = NULL;
	err = cudaMalloc((void **) &d_working_mat_copy,
			thidx * poly_num * 2 * sizeof(linearpart_t));
	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to allocate device working_mat_copy (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	squarepart_t *d_const_mat = NULL;
	err = cudaMalloc((void **) &d_const_mat,
			thidx * poly_num * sizeof(squarepart_t));
	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to allocate devices const_mat (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy oripolys from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	value_t h_result[thidx * RESULT_MAX_NUM * 2];
	for(int i = 0; i < thidx * RESULT_MAX_NUM * 2 ; i ++){

		h_result[i] = 0;
	}




	printf("finish allocate device memory!\n");


	int s_total_p0 = 0;
	value_t *val_arr = (value_t*)calloc(thidx, sizeof(value_t));
	linearpart_t *working_mat_copy_arr = (linearpart_t*)calloc(thidx * POLY_NUM * 2, sizeof(linearpart_t));
	squarepart_t *const_mat_arr = (squarepart_t*)calloc(thidx * POLY_NUM, sizeof(squarepart_t));
	char *bound_arr = (char*)calloc(thidx, sizeof(short));

	for (int thi = 0; thi < thidx; thi++) {

		value_t sol_num = 0;
		value_t solutions[SOL_MAX_NUM];

		//int sol_total = 0;
		value_t val = (set_val + (value_t) thi) << ENUM_NUM;

		val_arr[thi] = val;
		for (int pi = 0; pi < POLY_NUM; pi++) {
			working_mat[pi][0] = working_mat_file[pi][0];
			working_mat[pi][1] = working_mat_file[pi][1];
			const_mat[pi] = 0;
		}

		for (int pos = enum_num; pos < para_num; pos++) {

			if (val & ((value_t) 1 << pos)) {

				for (int pi = 0; pi < poly_num; pi++) {
					working_mat[pi][0] ^= linear_mat[pos][pi][0];
					working_mat[pi][1] ^= linear_mat[pos][pi][1];
				}

				for (int pi = 0; pi < poly_num; pi++) {
					const_mat[pi] ^= square_mat[pos][pi];

				}

			}

		}


		for (int i = 0; i < poly_num; i++) {
			working_mat_copy[i][0] = working_mat[i][0];
			working_mat_copy[i][1] = working_mat[i][1];

			working_mat_copy_arr[thi * POLY_NUM * 2 + 2 * i] = working_mat_copy[i][0];
			working_mat_copy_arr[thi * POLY_NUM * 2 + 2 * i + 1] = working_mat_copy[i][1];

			const_mat_arr[thi * POLY_NUM + i] = const_mat[i];

		}

		for (int pi = 0; pi < poly_num; pi++) {

			value_t w = const_mat[pi] & val;

			w = (w) ^ (w >> 32);
			w = (w) ^ (w >> 16);
			w = (w) ^ (w >> 8);
			w = (w) ^ (w >> 4);
			w = (w) ^ (w >> 2);
			w = (w) ^ (w >> 1);

			if (w & (value_t) 1) {

				working_mat[pi][0] ^= (linearpart_t) 1;
			}

		}

		sol_num = gauss_host(working_mat, POLY_NUM, UNKNOWN_NUM, solutions);
		s_total_p0 += sol_num;
		//verify the solutions
		for (int s = 0; s < sol_num; s++) {
			int one_num = 0;
			int zero_num = 0;
			int op;
			for (op = 0; op < ori_num; op++) {

				cstpoly[0] = polys[op][para_num + unknown_num][0];
				cstpoly[1] = polys[op][para_num + unknown_num][1];
				cstpoly[2] = polys[op][para_num + unknown_num][2];
				// for parameters.
				for (int pa = 0; pa < para_num; pa++) {
					if (val & ((value_t) 1 << pa)) {

						cstpoly[0] ^= polys[op][para_num - pa - 1][0];
						cstpoly[1] ^= polys[op][para_num - pa - 1][1];
						cstpoly[2] ^= polys[op][para_num - pa - 1][2];
					}
				}

				for (int un = 0; un < unknown_num; un++) {
					if (solutions[s] & ((value_t) 1 << un)) {
						cstpoly[0] ^= polys[op][para_num + un][0];
						cstpoly[1] ^= polys[op][para_num + un][1];
						cstpoly[2] ^= polys[op][para_num + un][2];
					}
				}

				cstpoly[2] ^= polys[op][unknown_num + para_num][2];
				// evaluate
				cstpoly[0] = cstpoly[0] & val;
				cstpoly[1] = cstpoly[1] & solutions[s];
				cstpoly[2] = cstpoly[2] & ((oripoly_t) 1);

				value_t w = cstpoly[0] ^ cstpoly[1] ^ cstpoly[2];

				w = (w) ^ (w >> 32);
				w = (w) ^ (w >> 16);
				w = (w) ^ (w >> 8);
				w = (w) ^ (w >> 4);
				w = (w) ^ (w >> 2);
				w = (w) ^ (w >> 1);

				if (w & (value_t) 1) {
					zero_num++;
					if (zero_num > ori_num - bound) {
						break;
					}

				} else {
					one_num++;
				}

			}
			if (zero_num + one_num == ori_num && one_num > bound) {
				//fprintf(out, "n=%d,val=0x%016x,sol=0x%016x\n",one_num, val, solutions[s]);
				h_result[thi * RESULT_MAX_NUM * 2] = val;
				h_result[thi * RESULT_MAX_NUM * 2 + 1] = solutions[s];

				fprintf(out,"n=%d,val=0x",one_num);
				//fprintf(out,"val=0x%016x,sol=0x%016x\n",h_result[i * 2 * RESULT_MAX_NUM + j * 2],h_result[i*2 * RESULT_MAX_NUM + j * 2 + 1]);
				fprintf(out, "%08x", (unsigned int)(val >> 32));
				fprintf(out, "%08x,", (unsigned int)(val & 0xFFFFFFFFULL));
				fprintf(out, "sol=0x");
				fprintf(out, "%08x", (unsigned int)(solutions[s] >> 32));
				fprintf(out, "%08x\n", (unsigned int)(solutions[s] & 0xFFFFFFFFULL));

			}
			if (one_num > bound) {
				bound = one_num;
				bound_arr[thi] = bound;
			}

		}

	}

	for(int i = 0; i < thidx;i++){
		if(bound_arr[i] < bound){
			bound_arr[i] = bound;
			h_result[i * RESULT_MAX_NUM * 2] = 0;
			h_result[i * RESULT_MAX_NUM * 2 + 1] = 0;

		}
	}

//	for(int i = 0; i < thidx;i++){
//		printf("No. %d, bound:%d, val:%lu, sol:%lu\n",i,bound_arr[i],h_result[i * RESULT_MAX_NUM * 2],h_result[i * RESULT_MAX_NUM * 2+1]);
//	}

	printf("finish cpu computing! the bound is %d now...\n", bound);

	//begin device part
	err = cudaMemcpy(d_val, val_arr, thidx * sizeof(value_t),
			cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		printf("Failed to copy value from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}


	err = cudaMemcpy(d_bound, bound_arr, thidx * sizeof(char),
			cudaMemcpyHostToDevice);
	if (err != cudaSuccess) {
		printf("Failed to copy bound from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	value_t *d_result = NULL;
	err = cudaMalloc((void **) &d_result,  thidx * RESULT_MAX_NUM * 2 * sizeof(value_t));
	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to allocate devices result (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}


	err = cudaMemcpy(d_result, h_result,
				thidx * RESULT_MAX_NUM * 2 * sizeof(value_t),cudaMemcpyHostToDevice);

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy result from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(d_working_mat_copy, working_mat_copy_arr,
			thidx * 2 * poly_num * sizeof(linearpart_t), cudaMemcpyHostToDevice);

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy working_mat_copy from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	err = cudaMemcpy(d_const_mat, const_mat_arr,
			thidx * poly_num * sizeof(squarepart_t), cudaMemcpyHostToDevice);

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy const_mat from host to device (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	printf("enum num : %d\nblock num : %d\nthread num : %d\n", ENUM_NUM,
			BLOCK_NUM, THREAD_NUM);

	cudaEvent_t start1;
	cudaEventCreate(&start1);
	cudaEvent_t stop1;
	cudaEventCreate(&stop1);
	cudaEventRecord(start1, NULL);

	printf("begin solve linear system!\n");
	solveLinear<<<BLOCK_NUM, THREAD_NUM>>>(d_working_mat_copy, d_const_mat,
			d_val, d_bound, d_sol_total,d_result);

	cudaEventRecord(stop1, NULL);
	cudaEventSynchronize(stop1);
	float msecTotal1 = 0.0f;
	cudaEventElapsedTime(&msecTotal1, start1, stop1);
	err = cudaGetLastError();

	if (err != cudaSuccess) {
		fprintf(stderr, "Failed to launch solveLinear kernel (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	value_t h_sol_total[thidx];
	err = cudaMemcpy(h_sol_total, d_sol_total, thidx * sizeof(value_t),
				cudaMemcpyDeviceToHost);

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy total solution numbers from device to host (error code %s)!\n",
				cudaGetErrorString(err));
			exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(h_result, d_result, thidx * RESULT_MAX_NUM * 2 * sizeof(value_t),
						cudaMemcpyDeviceToHost);

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to copy result from device to host (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaMemcpy(bound_arr, d_bound, thidx * sizeof(char),
					cudaMemcpyDeviceToHost);

	if (err != cudaSuccess) {
		fprintf(stderr,
					"Failed to copy bound from device to host (error code %s)!\n",
					cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

//	printf("\n------------ begin sort ---------------\n");
//	for(int i = 0; i < thidx; i++){
//		printf("bound:%d\n",bound_arr[i]);
//		printf("1. val:%lu, sol:%lu\n", h_result[i * 10],h_result[i*10 + 1]);
//		printf("2. val:%lu, sol:%lu\n", h_result[i * 10 + 2],h_result[i*10 + 3]);
//		printf("3. val:%lu, sol:%lu\n", h_result[i * 10 + 4],h_result[i*10 + 5]);
//		printf("4. val:%lu, sol:%lu\n", h_result[i * 10 + 6],h_result[i*10 + 7]);
//		printf("5. val:%lu, sol:%lu\n", h_result[i * 10 + 8],h_result[i*10 + 9]);
//
//	}



	for(int i = 0; i < thidx; i++){
		for(int j = i + 1; j < thidx; j++){
			if(bound_arr[i] > bound_arr[j]){
				char temp = bound_arr[i];
				bound_arr[i] = bound_arr[j];
				bound_arr[j] = temp;

				for(int ri = 0;ri < 2 * RESULT_MAX_NUM; ri ++){
					value_t temp = h_result[i * 2 * RESULT_MAX_NUM + ri];
					h_result[i * 2 * RESULT_MAX_NUM + ri] = h_result[j * 2 * RESULT_MAX_NUM + ri];
					h_result[j * 2 * RESULT_MAX_NUM + ri] = temp;
				}

			}
		}
	}

//	printf("\n------------ finish sort ---------------\n");

	printf("bound:%d\n",bound_arr[thidx-1]);
//	printf("1. val:%lu, sol:%lu\n", h_result[i * 10],h_result[i*10 + 1]);
//	printf("2. val:%lu, sol:%lu\n", h_result[i * 10 + 2],h_result[i*10 + 3]);
//	printf("3. val:%lu, sol:%lu\n", h_result[i * 10 + 4],h_result[i*10 + 5]);
//	printf("4. val:%lu, sol:%lu\n", h_result[i * 10 + 6],h_result[i*10 + 7]);
//	printf("5. val:%lu, sol:%lu\n", h_result[i * 10 + 8],h_result[i*10 + 9]);



	for(int i = thidx - 1;i >= 0;i--){
		for(int j = 0,k = 0; j < RESULT_MAX_NUM; j++, k++){
			if(h_result[i * 2 * RESULT_MAX_NUM + j * 2] != 0 && h_result[i*2 * RESULT_MAX_NUM + j * 2 + 1]!= 0){
				fprintf(out,"n=%d,val=0x",bound_arr[i]);
				//fprintf(out,"val=0x%016x,sol=0x%016x\n",h_result[i * 2 * RESULT_MAX_NUM + j * 2],h_result[i*2 * RESULT_MAX_NUM + j * 2 + 1]);
				 fprintf(out, "%08x", (unsigned int)(h_result[i * 2 * RESULT_MAX_NUM + j * 2] >> 32));
				 fprintf(out, "%08x,", (unsigned int)(h_result[i * 2 * RESULT_MAX_NUM + j * 2] & 0xFFFFFFFFULL));
				 fprintf(out, "sol=0x");
				 fprintf(out, "%08x", (unsigned int)(h_result[i*2 * RESULT_MAX_NUM + j * 2 + 1] >> 32));
				 fprintf(out, "%08x\n", (unsigned int)(h_result[i*2 * RESULT_MAX_NUM + j * 2 + 1] & 0xFFFFFFFFULL));
			}
//		fprintf(out,"2. val:%lu, sol:%lu\n", h_result[i * 2 * RESULT_MAX_NUM + 2],h_result[i*2 * RESULT_MAX_NUM + 3]);
		}
		if(bound_arr[i] > bound_arr[i-1]){
			break;
		}
	}




	printf("time:%.3lf ms\n---------------------------------------\n", msecTotal1);


	cudaFree(d_working_mat_copy);
	cudaFree(d_const_mat);
	cudaFree(d_val);
	cudaFree(d_bound);
	cudaFree(d_sol_total);
	cudaFree(d_result);
}
