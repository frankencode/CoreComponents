#include <stdio.h>

int main() {
    #if __cplusplus <= 199711L
    printf("{compile-flags: -std=c++11}");
    #else
    printf("{}");
    #endif
    return 0;
}
