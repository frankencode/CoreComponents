#include <cc/stdio>
#include <cc/Arguments>
#include <cc/ui/FtFontFace>

using namespace cc;
using namespace cc::ui;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->baseName();
    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv, VariantMap::create());
        for (String path: arguments->items()) {
            Ref<FtFontFace> face = FtFontFace::open(path);
            fout()
                << path << ":" << nl
                << "  family  : " << face->family() << nl
                << "  weight  : " << face->weight() << nl
                << "  slant   : " << face->slant() << nl
                << "  stretch : " << face->stretch() << nl
                << "  pitch   : " << face->pitch() << nl
                << nl;
        }
    }
    catch (HelpRequest &) {
        fout()
            << "Usage: " << toolName << " [FILE]..." << nl
            << nl
            << "Print information about font files." << nl;
    }
    return 0;
}
