#include <cc/System>
#include <cc/Date>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    if (argc == 1) {
        fout() << static_cast<int64_t>(System::now()) << nl;
    }
    else {
        for (int i = 1; i < argc; ++i) {
            fout() << Date { String{argv[i]}.toDouble() } << nl;
        }
    }
    return 0;
}
