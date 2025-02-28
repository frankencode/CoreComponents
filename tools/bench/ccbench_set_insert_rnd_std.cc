#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <cc/DEBUG>
#include <set>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    Array<long> items = Array<long>::allocate(n);

    fout() << "Test preparation: generate shuffled random numbers... ";
    {
        double t = System::now();

        Random random { 0 };

        for (int i = 0; i < n; ++i) {
            items[i] = random();
        }

        for (int i = 0; i < 3; ++i) {
            random.shuffle(items);
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    std::set<long> numbers;

    fout() << n << " random insertions into to std::set<long>... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            numbers.insert(items[i]);
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " random lookups... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            if (!numbers.contains(items[i])) {
                ferr() << "FAILED" << nl;
                return 1;
            }
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    CC_INSPECT(sizeof(numbers));

    return 0;
}
