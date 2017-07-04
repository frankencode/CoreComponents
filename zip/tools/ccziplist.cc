#include <cc/stdio>
#include <cc/zip/ZipArchive>

using namespace cc;
using namespace cc::zip;

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        Ref<ZipArchive> archive = ZipArchive::open(argv[i]);
        for (String path; archive->read(&path);)
            fout() << path << nl;
    }
    return 0;
}
