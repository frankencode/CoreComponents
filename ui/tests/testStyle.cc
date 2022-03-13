#include <cc/ui/FontManager>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::ui;

    Set<FontFamily> families = FontManager{}.fontFamilies();
    for (const FontFamily &family: families) {
        List<FontFace> faces = family.fontFaces();
        for (const auto &face: faces) {
            fout() << face << nl << nl;
        }
    }

    return 0;
}
