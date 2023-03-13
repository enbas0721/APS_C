#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    long int* a;
    a = (long int*)malloc(10*sizeof(long int));
    for (size_t i = 0; i < 10; i++) a[i] = 0;
    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 2147483649; j++)
        {
            a[i] += 1000;
        }
        printf("a[i]:%ld\n",a[i]);
    }
    
    return 0;
}
