#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <cc/DEBUG>
#include <cmath>
#include <map>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    std::map<long, long> numbers;
    {
        fout() << n << " ascending insertion into std::map<long, long>... ";
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            numbers.insert({i, i});
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    fout() << n << " lookups... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            if (!numbers.contains(i)) return 1;
        }

        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    CC_INSPECT(sizeof(std::map<long, long>));

    return 0;
}
