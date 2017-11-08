#include <stdio.h>

int main() {
    #if __cplusplus < 201402L
    printf("{compile-flags: -std=c++14}");
    #else
    printf("{}");
    #endif
    return 0;
}
