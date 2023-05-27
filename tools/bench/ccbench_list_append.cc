#include <cc/System>
#include <cc/List>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    const int n = argc > 1 ? String{argv[1]}.toInt() : 1000000;

    List<long> numbers;

    fout() << "Appending to List<long>... ";
    {
        double t = System::now();

        for (int i = 0; i < n; ++i) {
            numbers.pushBack(i);
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

    fout() << "sizeof(List<long>) = " << sizeof(List<long>) << nl;
    fout() << "sizeof(List<long>::Tree) = " << sizeof(List<long>::Tree) << nl;

    return 0;
}
