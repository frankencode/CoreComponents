#include <cc/malloc>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

inline static int random_get(const int a, const int b)
{
    const unsigned m = (1u << 31) - 1;
    static unsigned x = 7;
    x = (16807 * x) % m;
    return ((uint64_t)x * (b - a)) / (m - 1) + a;
}

static double time_get()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int main(int argc, char **argv)
{
    const int n = 10000000;

    const int size_min = 12;
    const int size_max = 130;

    uint8_t **buf = malloc(n * sizeof(uint8_t *));
    int *buf_size = malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i) {
        buf_size[i] = random_get(size_min, size_max);
        buf[i] = malloc(buf_size[i]);
    }

    printf("kiss malloc()/free() benchmark\n");
    printf("------------------------------\n");

    {
        double t = time_get();

        for (int i = 0; i < n; ++i) {
            buf_size[i] = random_get(size_min, size_max);
            buf[i] = malloc(buf_size[i]);
        }

        t = time_get() - t;

        printf("malloc() burst speed:\n");
        printf("  n = %d (number of allocations)\n", n);
        printf("  t = %f s (test duration)\n", t);
        printf("  n/t = %f MHz (average number of allocations per second)\n", n/t / 1e6);
        printf("  t/n = %f us (average latency of an allocation)\n", t/n * 1e6);
        printf("\n");
    }

    {
        double t = time_get();

        for (int i = n - 1; i >= 0; --i)
            free(buf[i]);

        t = time_get() - t;

        printf("free() burst speed:\n");
        printf("  n = %d (number of deallocations)\n", n);
        printf("  t = %f s (test duration)\n", t);
        printf("  n/t = %f MHz (average number of deallocations per second)\n", n/t / 1e6);
        printf("  t/n = %f us (average latency of a deallocation)\n", t/n * 1e6);
        printf("\n");
    }

    return 0;
}
