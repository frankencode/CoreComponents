#include <cc/System>
#include <cc/stdio>
#include <list>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 10000000;

    std::list<long> numbers;

    fout() << "Appending to std::list<long>... ";
    {
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            numbers.push_back(i);
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }

    /*fout() << n << " lookups... ";
    {
        double t = System::now();
        for (int i = 0; i < n; ++i) {
            if (numbers.at(i) != i) return 1;
        }
        t = System::now() - t;
        fout() << std::round(t * 1000) << " ms\n";
    }*/

    fout() << "sizeof(std::list<long>) = " << sizeof(std::list<long>) << nl;

    return 0;
}
