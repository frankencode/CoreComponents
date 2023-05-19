#include <cc/System>
#include <cc/Random>
#include <cc/Set>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 100000;

    Set<long> numbers;

    fout() << n << " random insertios into cc::Set<int>... ";
    {
        Random random { 0 };
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            long r = random();
            numbers.insert(r);
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " random lookups... ";
    {
        Random random { 0 };
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            long r = random();
            if (!numbers.contains(r)) {
                ferr() << "FAILED" << nl;
                return 1;
            }
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    return 0;
}
