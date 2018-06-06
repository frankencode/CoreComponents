#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <cc/stdio>
#include <cc/Arguments>
#include <cc/ui/FtFontFace>

using namespace cc;
using namespace cc::ui;

void printFamilyInfo(const Arguments *arguments)
{
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

void printGlyphLists(const Arguments *arguments)
{
    for (String path: arguments->items()) {
        Ref<FtFontFace> face = FtFontFace::open(path);
        FT_Face ftFace = face->ftFace();

        FT_UInt glyphIndex = 0;
        FT_ULong ch = FT_Get_First_Char(ftFace, &glyphIndex);
        String buffer = String::allocate(256);
        while (glyphIndex != 0) {
            String glyphName;
            if (FT_HAS_GLYPH_NAMES(ftFace)) {
                if (FT_Get_Glyph_Name(ftFace, glyphIndex, mutate(buffer)->bytes(), buffer->count()) == 0)
                    glyphName = buffer->copy(0, buffer->find('\0'));
            }
            fout() << glyphIndex << " (\\u" << hex(ch) << ", " << "'" << (ch < 0x80 ? char(ch) : ' ') << "'): \"" << glyphName << "\"" << nl;
            ch = FT_Get_Next_Char(ftFace, ch, &glyphIndex);
        }
    }
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->baseName();
    try {
        Ref<VariantMap> options = VariantMap::create();
        options->insert("family-info", false);
        options->insert("glyph-list", false);
        Ref<Arguments> arguments = Arguments::parse(argc, argv, options);

        if (options->value("family-info"))
            printFamilyInfo(arguments);
        else if (options->value("glyph-list"))
            printGlyphLists(arguments);
        else
            printFamilyInfo(arguments);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]...\n"
            "\n"
            "Options:\n"
            "  -family-info   print information about the font family (default)\n"
            "  -glyph-list    list all glyphs\n"
            "\n"
            "Print information about font files.\n"
        ) << toolName;
    }
    return 0;
}
