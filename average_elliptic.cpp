#include <iostream>
#include <cstring>
#include <fstream>
#include "big.h"
#include <time.h>
#include "ecn.h"
#include <unistd.h>


using namespace std;

#define runtime 100

Miracl precision(50,0);
char *ecp=(char *)"D35E472036BC4FB7E13C785ED201E065F98FCFA6F6F40DEF4F92B9EC7893EC28FCD412B1F1B32E27";
char *eca=(char *)"3EE30B568FBAB0F883CCEBD46D3F3BB8A2A73513F5EB79DA66190EB085FFA9F492F375A97D860EB4";
char *ecb=(char *)"520883949DFDBC42D3AD198640688A6FE13F41349554B49ACC31DCCD884539816F5EB4AC8FB1F1A6";
/* elliptic curve - point of prime order (x,y) */
char *ecx=(char *)"43BD7E9AFB53D8B85289BCC48EE5BFE6F20137D10A087EB6E7871E2A10A599C710AF8D0D39E20611";
char *ecy=(char *)"14FDD05545EC1CC8AB4093247F77275E0743FFED117182EAA9C77877AAAC6AC7D35245D1692E8EE1";

int main()
{

    clock_t start, end;
    double max,min,sum,average,result;
    double time1[100];
    time_t seed;
    
    Big a,b,p,q,d,x,y,x1,y1,x2,y2,c;
    ECn g,g2,ea,eb,erst;
    miracl *mip=&precision;

    time(&seed);
    irand((long)seed);

    // Generate an elliptic curve
    mip->IOBASE=16;
    a=eca;
    b=ecb;
    p=ecp;
    ecurve(a,b,p,MR_BEST);  // means use PROJECTIVE if possible, else AFFINE coordinates
    x=ecx;
    y=ecy;
    mip->IOBASE=10;
    g=ECn(x,y);
    ea=eb=g;

    cout<<"ECC point*"<<endl;
    
    
    result=0;
    for (int x = 0; x < runtime; x++){
    	d = rand(160,2);
    	
    	sleep(0.1);
    	
        start = clock(); //Start measuring performance
        ea*=d;
        end = clock(); //End measuring performance
        
        time1[x] = (double)(end-start);  //Store time in array time1[.]       
    }
    
    max = time1[0];
    min = time1[0];
    sum = 0;
    for(int j = 0; j < runtime; j++){
        if(time1[j]>=max) max=time1[j]; // recording max time
        if(time1[j]<=min) min=time1[j]; // recording min time
        sum+=time1[j];
    }
    
    average = sum/runtime; // Calculating average time
    printf("max time : %.3f ms \n", (max/CLOCKS_PER_SEC)*1000);
    printf("min time : %.3f ms \n", (min/CLOCKS_PER_SEC)*1000);
    printf("average time : %.3f ms \n", (average/CLOCKS_PER_SEC)*1000);
    return 0;
}
