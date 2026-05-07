#include <iostream>
#include <cstring>
#include "big.h"
#include <time.h>
#include <unistd.h>

#define RUNTIME       100
#define CHALLENGE_LEN 32
#define RESPONSE_LEN  32
#define HASH_LEN      32
#define SYNDROME_LEN  32
#define NOISE_BITS    2

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

void PUF_Response(const char *challenge, int len, char *response, unsigned int device_seed) {
    unsigned int seed = device_seed;
    for (int i = 0; i < len; i++)
        seed ^= ((unsigned int)challenge[i] << (i % 24));
    srand(seed);
    for (int i = 0; i < RESPONSE_LEN; i++)
        response[i] = (char)(rand() % 256);
    srand((unsigned int)time(0));
}

void AddNoise(const char *input, char *output, int len) {
    memcpy(output, input, len);
    for (int n = 0; n < NOISE_BITS; n++) {
        int pos = rand() % len;
        int bit = rand() % 8;
        output[pos] ^= (1 << bit);
    }
}

void FuzzyExtractor_Gen(const char *response, int len, char *K, char *syndrome) {
    ComputeHash(response, len, K);
    for (int i = 0; i < len; i++)
        syndrome[i] = response[i] ^ K[i % HASH_LEN];
}

void FuzzyExtractor_Rep(const char *response_noisy, const char *syndrome, int len, char *K_recover) {
    char corrected[RESPONSE_LEN];
    for (int i = 0; i < len; i++)
        corrected[i] = response_noisy[i] ^ syndrome[i];
    ComputeHash(corrected, len, K_recover);
}

int main() {
    char challenge[CHALLENGE_LEN];
    char response[RESPONSE_LEN];
    char response_noisy[RESPONSE_LEN];
    char K[HASH_LEN];
    char syndrome[SYNDROME_LEN];
    char K_recover[HASH_LEN];
    double elapsed[RUNTIME];
    double max_t, min_t, sum, average;
    clock_t start, end;
    int match_count = 0;

    unsigned int device_seed = 0xDEADBEEF;

    InitializeRandomSeed();
    printf("PUF + Fuzzy Extractor\n");
    printf("Challenge: %d bytes / Noise: %d bits\n\n", CHALLENGE_LEN, NOISE_BITS);

    for (int x = 0; x < RUNTIME; x++) {
        for (int z = 0; z < CHALLENGE_LEN; z++)
            challenge[z] = GetRandomCharacter();

        PUF_Response(challenge, CHALLENGE_LEN, response, device_seed);
        AddNoise(response, response_noisy, RESPONSE_LEN);

        start = clock();
        FuzzyExtractor_Gen(response, RESPONSE_LEN, K, syndrome);
        FuzzyExtractor_Rep(response_noisy, syndrome, RESPONSE_LEN, K_recover);
        end = clock();

        elapsed[x] = (double)(end - start);

        if (memcmp(K, K_recover, HASH_LEN) == 0)
            match_count++;
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