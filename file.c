#include<stdio.h>
#define count(num) ({\
                int count=0;\
                while(num)\
                {\
                num=num&(num-1);\
                count++;\
                }\
                count;\
                })
#define P1() 3.14
#define PI 3.14
#define s prasad
#define av int
int main()
{
av x=9;
char *p="s";
printf("%d\n",count(x) );
printf("%f s\n", PI );
printf("%f\n",P1());
}

