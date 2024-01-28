#include <cc/ZipFile>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    for (int i = 1; i < argc; ++i) {
        ZipFile archive { argv[i] };
        for (String path: archive) {
            fout() << path << nl;
        }
    }

    return 0;
}
