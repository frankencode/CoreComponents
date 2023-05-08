#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <set>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 100000;

    std::set<long> numbers;

    fout() << "Ordered insertion of sparse random numbers into to std::set<long>... ";
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
