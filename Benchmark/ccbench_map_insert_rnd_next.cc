#include <cc/private/bucket_tree>
#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <cmath>

int main(int argc, char *argv[])
{
    using namespace cc;
    using bucket_tree::Map;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    Map<int> numbers;

    fout() << n << " random insertion into cc::Map<int>... ";
    {
        Random random;
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            int r = random();
            numbers.insert(r, i);
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " random lookups... ";
    {
        Random random;
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            int r = random();
            if (!numbers.contains(r)) return 1;
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    return 0;
}
