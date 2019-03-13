
#include <stdio.h>
#include <memory.h>
typedef unsigned char UINT8;
typedef unsigned long long UINT64;
typedef UINT64 tKeccakLane;

#define KeccakReferences
#define maxNrRounds 24
#define reducedRound 3
#define nrLanes 25
#define index(x, y) (((x)%5)+5*((y)%5))
#define KeccakP1600_stateSizeInBytes    200
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define UINT64_MAX ULLONG_MAX


void KeccakP1600_InitializeRoundConstants(void);
void KeccakP1600_InitializeRhoOffsets(void);
static int LFSR86540(UINT8 *LFSR);

void KeccakP1600_Initialize(void *state);
void KeccakP1600_StaticInitialize(void);


void KeccakP1600_DisplayRoundConstants(FILE *f);
void KeccakP1600_DisplayRhoOffsets(FILE *f);
void displayStateAs64bitLanes(int level, const char *text, void *statePointer);
void KeccakF1600Permute(tKeccakLane *state, unsigned int nRounds);

void Keccak(unsigned int rate, unsigned int capacity, const UINT64 *input, unsigned int inputBitLen, UINT64 *output, unsigned int outputBitLen, int level);

