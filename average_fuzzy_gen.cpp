#include <iostream>
#include <cstring>
#include "big.h"
#include <time.h>
#include <unistd.h>

#define RUNTIME   100
#define INPUT_LEN 32
#define HASH_LEN  32
#define NOISE_BITS 2

using namespace std;

void InitializeRandomSeed() {
    srand((unsigned int)time(0));
}

char GetRandomCharacter() {
    return 'a' + (rand() % 26);
}

void ComputeHash(const char *input, int len, char *output) {
    sha256 sh256;
    shs256_init(&sh256);
    for (int i = 0; i < len; i++)
        shs256_process(&sh256, input[i]);
    shs256_hash(&sh256, output);
}

void FuzzyExtractor_Gen(const char *w, int len, char *K, char *P) {
    memcpy(P, w, len);
    ComputeHash(w, len, K);
}

void FuzzyExtractor_Rep(const char *w_noisy, const char *P, int len, char *K_recover) {
    char corrected[INPUT_LEN];
    for (int i = 0; i < len; i++)
        corrected[i] = P[i];
    ComputeHash(corrected, len, K_recover);
}

void AddNoise(const char *input, char *output, int len) {
    memcpy(output, input, len);
    for (int n = 0; n < NOISE_BITS; n++) {
        int pos = rand() % len;
        int bit = rand() % 8;
        output[pos] ^= (1 << bit);
    }
}

int main() {
    char K[HASH_LEN];
    char P[INPUT_LEN];
    char K_recover[HASH_LEN];
    double elapsed[RUNTIME];
    double max_t, min_t, sum, average;
    clock_t start, end;
    int match_count = 0;

    InitializeRandomSeed();
    printf("Fuzzy Extractor\n");

    for (int x = 0; x < RUNTIME; x++) {
        char *w = new char[INPUT_LEN];
        for (int z = 0; z < INPUT_LEN; z++)
            w[z] = GetRandomCharacter();

        char w_noisy[INPUT_LEN];
        AddNoise(w, w_noisy, INPUT_LEN);

        start = clock();
        FuzzyExtractor_Gen(w, INPUT_LEN, K, P);
        FuzzyExtractor_Rep(w_noisy, P, INPUT_LEN, K_recover);
        end = clock();

        elapsed[x] = (double)(end - start);

        if (memcmp(K, K_recover, HASH_LEN) == 0)
            match_count++;

        delete[] w;
    }

    max_t = elapsed[0];
    min_t = elapsed[0];
    sum   = 0;

    for (int j = 0; j < RUNTIME; j++) {
        if (elapsed[j] >= max_t) max_t = elapsed[j];
        if (elapsed[j] <= min_t) min_t = elapsed[j];
        sum += elapsed[j];
    }
    average = sum / RUNTIME;

    printf("Key recovery rate : %d / %d (%.1f%%)\n\n", match_count, RUNTIME,
           (double)match_count / RUNTIME * 100);
    printf("max time     : %.6f ms\n", (max_t / CLOCKS_PER_SEC) * 1000);
    printf("min time     : %.6f ms\n", (min_t / CLOCKS_PER_SEC) * 1000);
    printf("average time : %.6f ms\n", (average / CLOCKS_PER_SEC) * 1000);

    return 0;
}