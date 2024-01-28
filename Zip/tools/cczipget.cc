#include <cc/ZipFile>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    if (argc != 3) {
        fout() << "Usage: " << String{argv[0]}.baseName() << " <ZIP ARCHIVE> <file name>" << nl;
        return 1;
    }

    ZipFile{argv[1]}.open(argv[2]).transferTo(IoStream::output());

    return 0;
}
