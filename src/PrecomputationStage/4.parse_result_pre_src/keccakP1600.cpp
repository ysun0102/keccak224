

#include "keccakP1600.h"


static tKeccakLane KeccakRoundConstants[maxNrRounds];
static unsigned int KeccakRhoOffsets[nrLanes];
FILE *intermediateValueFile = fopen("testKeccak224Preimage.txt","w+");
int displayLevel = 10;

void displayStateAs64bitLanes(int level, const char *text, void *statePointer)
{
    unsigned int i;
    unsigned long long int *state = (unsigned long long int *)statePointer;
    
    if ((intermediateValueFile) && (level <= displayLevel)) {
        fprintf(intermediateValueFile, "%s:\n", text);
        for(i=0; i<25; i++) {
            fprintf(intermediateValueFile, "%08X", (unsigned int)(state[i] >> 32));
            fprintf(intermediateValueFile, "%08X", (unsigned int)(state[i] & 0xFFFFFFFFULL));
            if ((i%5) == 4)
                fprintf(intermediateValueFile, "\n");
            else
                fprintf(intermediateValueFile, " ");
        }
    }
    
    if (level <= displayLevel) {
        printf( "%s:\n", text);
        for(i=0; i<25; i++) {
            printf( "%08X", (unsigned int)(state[i] >> 32));
            printf( "%08X", (unsigned int)(state[i] & 0xFFFFFFFFULL));
            if ((i%5) == 4)
                printf( "\n");
            else
                printf( " ");
        }
    }
}


void KeccakP1600_StaticInitialize(void)
{
    if (sizeof(tKeccakLane) != 8) {
        printf("tKeccakLane should be 64-bit wide\n");
        //abort();
    }
    KeccakP1600_InitializeRoundConstants();
    KeccakP1600_InitializeRhoOffsets();
}

void KeccakP1600_InitializeRoundConstants(void)
{
    UINT8 LFSRstate = 0x01;
    unsigned int i, j, bitPosition;
    
    for(i=0; i<maxNrRounds; i++) {
        KeccakRoundConstants[i] = 0;
        for(j=0; j<7; j++) {
            bitPosition = (1<<j)-1; /* 2^j-1 */
            if (LFSR86540(&LFSRstate))
                KeccakRoundConstants[i] ^= (tKeccakLane)1<<bitPosition;
        }
    }
}

void KeccakP1600_InitializeRhoOffsets(void)
{
    unsigned int x, y, t, newX, newY;
    
    KeccakRhoOffsets[index(0, 0)] = 0;
    x = 1;
    y = 0;
    for(t=0; t<24; t++) {
        KeccakRhoOffsets[index(x, y)] = ((t+1)*(t+2)/2) % 64;
        newX = (0*x+1*y) % 5;
        newY = (2*x+3*y) % 5;
        x = newX;
        y = newY;
    }
}

static int LFSR86540(UINT8 *LFSR)
{
    int result = ((*LFSR) & 0x01) != 0;
    if (((*LFSR) & 0x80) != 0)
    /* Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1 */
        (*LFSR) = ((*LFSR) << 1) ^ 0x71;
    else
        (*LFSR) <<= 1;
    return result;
}

void KeccakP1600_Initialize(void *state)
{
    memset(state, 0, 1600/8);
}



#define ROL64(a, offset) ((offset != 0) ? ((((tKeccakLane)a) << offset) ^ (((tKeccakLane)a) >> (64-offset))) : a)

static void theta(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane C[5], D[5];
    
    for(x=0; x<5; x++) {
        C[x] = 0;
        for(y=0; y<5; y++)
            C[x] ^= A[index(x, y)];
    }
    for(x=0; x<5; x++)
        D[x] = ROL64(C[(x+1)%5], 1) ^ C[(x+4)%5];
    for(x=0; x<5; x++)
        for(y=0; y<5; y++)
            A[index(x, y)] ^= D[x];
}

static void rho(tKeccakLane *A)
{
    unsigned int x, y;
    
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(x, y)] = ROL64(A[index(x, y)], KeccakRhoOffsets[index(x, y)]);
}

static void pi(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane tempA[25];
    
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        tempA[index(x, y)] = A[index(x, y)];
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(0*x+1*y, 2*x+3*y)] = tempA[index(x, y)];
}

static void chi(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane C[5];
    
    for(y=0; y<5; y++) {
        for(x=0; x<5; x++)
            C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
        for(x=0; x<5; x++)
            A[index(x, y)] = C[x];
    }
}

static void iota(tKeccakLane *A, unsigned int indexRound)
{
    A[index(0, 0)] ^= KeccakRoundConstants[indexRound];
}


void KeccakP1600Round(tKeccakLane *state, unsigned int indexRound)
{
//#ifdef KeccakReference
//#endif
    //displayStateAs64bitLanes(0, "\nbefore theta", state);

    theta(state);

    //displayStateAs64bitLanes(0, "after theta", state);

    
    rho(state);

    //displayStateAs64bitLanes(0, "after rho", state);

    
    pi(state);

    //displayStateAs64bitLanes(0, "after pi", state);

    
    chi(state);

    //displayStateAs64bitLanes(0, "after chi", state);

    
    iota(state, indexRound);

    //displayStateAs64bitLanes(0, "after iota", state);

}

void KeccakF1600Permute(tKeccakLane *state, unsigned int nRounds){
    for (int n = 0; n < nRounds; n++) {
        KeccakP1600Round(state, n);
    }
}


void Keccak(unsigned int rate, unsigned int capacity, const UINT64 *input, unsigned int inputBitLen, UINT64 *output, unsigned int outputBitLen, int level){
    
    tKeccakLane state[25];
    unsigned int blockSize = 0;
    unsigned int i;
    unsigned rateInWords = rate / 64;
//    unsigned inputInWords = inputBitLen / 64;
    
    
    if((rate + capacity) != 1600 || (rate % 8) != 0){
        printf("rate or capacity is illegal.\n");
        return;
    }
    
    
    
    /*  Initialize the state */
    KeccakP1600_StaticInitialize();
    KeccakP1600_Initialize(state);

    displayStateAs64bitLanes(level, "Init", state);

    
    
    /*  Absorb all the input blocks */
    int pos = 0;
    int blocknum = 0;
    while (inputBitLen > pos) {
        blocknum ++;
        blockSize = MIN(inputBitLen- pos, rate);
        for (i = 0; i < blockSize; i++) {
            UINT64 mask = (UINT64)1<< ((pos + i) % 64);
            state[i/ 64] ^= input[(pos+i) / 64] & mask;
        }
        pos += blockSize;
        displayStateAs64bitLanes(level, "Absorb with message", state);

        
        if(blockSize == rate){
            KeccakF1600Permute(state, reducedRound);
            displayStateAs64bitLanes(level, "After Keccak", state);

            blockSize = 0;
        }
    }
    displayStateAs64bitLanes(level, "Before padding", state);
    
    /* Do the padding and switch to the squeezing phase */
    /* Add the first bit of padding */
    state[blockSize /64 ] ^= (UINT64) 1 << (blockSize % 64);
    /* If the first bit of padding is at position rate - 1, we need a whole new block for the second bit of padding */
    displayStateAs64bitLanes(level, "After first bit of padding", state);
    

    for (int i = 0; i <= 25 ; i++) {
        printf("%llx ",state[i]);
    }
    printf("\n");
    /* Add the second bit of padding */
    state[rateInWords - 1] ^= (UINT64) 1 << 63;
    displayStateAs64bitLanes(level, "After second bit of padding", state);
    /* Switch to the squeezing phase */
    KeccakF1600Permute(state, reducedRound);
    displayStateAs64bitLanes(level, "After keccak", state);
    /* Squeeze out all the output blocks */
    pos = 0;
    while(outputBitLen - pos > 0) {
        blockSize = MIN(outputBitLen, rate);
        for (i = 0; i < blockSize; i++) {
            UINT64 mask = (UINT64)1<< (i % 64);
            output[(pos + i)/ 64] ^= (state[i/ 64] & mask) << (pos % 64);
        }
        pos += blockSize;
        
        
        if (outputBitLen - pos > 0){
            KeccakF1600Permute(state,reducedRound);
            blockSize = 0;
        }
    }
    
    
    
}
