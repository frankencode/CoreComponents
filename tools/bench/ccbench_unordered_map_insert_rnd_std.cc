#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <cmath>
#include <map>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 100000;

    std::unordered_map<long, long> numbers;

    fout() << n << " random insertion into std::unordered_map<long>... ";
    {
        Random random;
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            int r = random();
            numbers.insert({r, i});
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

    fout() << "numbers.bucket_count = " << numbers.bucket_count() << nl;
    fout() << "numbers.load_factor = " << numbers.load_factor() << nl;
    fout() << "numbers.size / numbers.bucket_count = " << static_cast<double>(numbers.size()) / numbers.bucket_count() << nl;

    return 0;
}
