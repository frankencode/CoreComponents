#include <cc/Random>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    Random random { 0 };
    uint32_t r0 = random();
    unsigned long n = 1;
    while (random() != r0) ++n;
    fout() << "n = " << n << nl;

    return 0;
}
