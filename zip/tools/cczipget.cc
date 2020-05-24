#include <cc/stdio>
#include <cc/zip/ZipArchive>

using namespace cc;
using namespace cc::zip;

int main(int argc, char **argv)
{
    if (argc != 3) {
        fout() << "Usage: " << String{argv[0]}->baseName() << " <ZIP ARCHIVE> <file name>" << nl;
        return 1;
    }

    ZipArchive::open(argv[1])->openFile(argv[2])->transferTo(standardOutput());

    return 0;
}
