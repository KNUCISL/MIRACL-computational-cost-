#include <iostream>
#include <cstring>
#include <fstream>
#include "big.h"
#include <time.h>
#include <unistd.h>

#define runtime 100

using namespace std;

void InitializeRandomSeed()
{
	srand((unsigned int)time(0));
}

char GetRandomCharacter()
{
	return 'a' + (rand() % 26);
}

int main()
{
    char hash[10000];
    double time[100];
    int i; 
    sha256 sh256;
    clock_t start, end;
    double max,min,sum,average;    
    int length;
    
    length = 32;

    InitializeRandomSeed();
    printf("Hash function\n");

    for (int x = 0; x < runtime; x++)
    {
        char *test = new char(length); //New array

        for (int z = 0; z < length; z++)
        {
        test[z]=GetRandomCharacter(); //Randomized plain text
        }
        
        sleep(0.5);
        
        start = clock(); //Start measuring performance
        shs256_init(&sh256);
        for (i=0;test[i]!=0;i++) shs256_process(&sh256,test[i]);
        shs256_hash(&sh256,hash);                
        end = clock(); //End measuring performance
    
        time[x]=(double)(end-start); //Store time in array time[.] 
        delete[] test;
    }

    max=time[0];
    min=time[0];
    sum=0;
    for (int j = 0; j < runtime; j++)
    {
        if(time[j]>=max) max=time[j]; // recording max time
        if(time[j]<=min) min=time[j]; // recording min time
        sum+=time[j]; 
    }
    average=sum/runtime; // Calculating average time
    
    printf("max time : %.3f ms \n", (max/CLOCKS_PER_SEC)*1000);
    printf("min time : %.3f ms \n", (min/CLOCKS_PER_SEC)*1000);
    printf("average time : %.3f ms \n", (average/CLOCKS_PER_SEC)*1000);
    printf("\n");

    return 0;
}
