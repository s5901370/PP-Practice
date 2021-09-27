#include "utils.h"
#define MAX 128
int ret[128];
int run(int n, int key) {
    int sum = 0;
    int a = MAX/4;
    f(n, key, ret, ret+a, ret+2*a, ret+3*a); //ret, ret+1, ret+2, ret+3
    for (int i = 0; i < 4; i++)
        sum += ret[i*a];
    return sum;
}