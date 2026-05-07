#include <iostream>
#include <cstring>
#include "big.h"
#include <time.h>
#include <unistd.h>

#define RUNTIME       100
#define CHALLENGE_LEN 32
#define RESPONSE_LEN  32
#define HASH_LEN      32
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

void FuzzyExtractor_Gen(const char *response, int len, char *K, char *P) {
    memcpy(P, response, len);
    