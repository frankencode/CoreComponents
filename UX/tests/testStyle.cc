#include <cc/FontManager>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc;

    Set<FontFamily> families = FontManager{}.fontFamilies();
    for (const FontFamily &family: families) {
        List<FontFace> faces = family.fontFaces();
        for (const auto &face: faces) {
            fout() << face << nl << nl;
        }
    }

    return 0;
}
