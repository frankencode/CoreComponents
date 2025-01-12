#include <cc/System>
#include <cc/Random>
#include <cc/stdio>
#include <set>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    std::set<long> numbers;

    fout() << "Ascending insertion into to std::set<long>... ";
    {
        long r = 0;
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            numbers.insert(r);
            ++r;
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    return 0;
}
