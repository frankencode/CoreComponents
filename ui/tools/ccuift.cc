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

String camelize(const String &name)
{
    String s = name;
    if (s->count() == 0) return s;
    mutate(s)->at(0) = toUpper(s->at(0));
    for (int i = 1; i < s->count() - 1; ++i) {
        if (s->at(i) == '_' || s->at(i) == '-')
            mutate(s)->at(i + 1) = toUpper(mutate(s)->at(i + 1));
    }
    return s->replace("_", "")->replace("-", "");
}

typedef Map<String, uchar_t> GlyphMap;

Ref<GlyphMap> parseFile(const String &path, int *maxKeyLength)
{
    Ref<FtFontFace> face = FtFontFace::open(path);
    FT_Face ftFace = face->ftFace();
    if (!FT_HAS_GLYPH_NAMES(ftFace)) {
        ferr() << path << ": Has no glyph names -- skipped" << nl;
        return nullptr;
    }

    auto map = GlyphMap::create();

    FT_UInt glyphIndex = 0;
    FT_ULong ch = FT_Get_First_Char(ftFace, &glyphIndex);
    String buffer = String::allocate(256);
    while (glyphIndex != 0) {
        String glyphName;
        if (FT_Get_Glyph_Name(ftFace, glyphIndex, mutate(buffer)->bytes(), buffer->count()) == 0)
            glyphName = buffer->copy(0, buffer->find('\0'));
        // fout() << glyphIndex << " (\\u" << hex(ch) << ", " << "'" << (ch < 0x80 ? char(ch) : ' ') << "'): \"" << glyphName << "\"" << nl;
        glyphName = camelize(glyphName);
        map->insert(glyphName, ch);
        if (glyphName->count() > *maxKeyLength)
            *maxKeyLength = glyphName->count();
        ch = FT_Get_Next_Char(ftFace, ch, &glyphIndex);
    }

    return map;
}

void printIconEnum(const Arguments *arguments)
{
    for (String path: arguments->items())
    {
        int maxKeyLength = 0;
        auto map = parseFile(path, &maxKeyLength);
        if (!map) continue;

        fout("enum class Icon: uchar_t {\n");
        for (int i = 0; i < map->count(); ++i) {
            fout("    %% = 0x%%%%\n")
                << left(map->keyAt(i), maxKeyLength)
                << hex(map->valueAt(i))
                << (i < map->count() - 1 ? "," : "");
        }
        fout("};\n");
    }
}

void printIconStr(const Arguments *arguments)
{
    for (String path: arguments->items())
    {
        int maxKeyLength = 0;
        auto map = parseFile(path, &maxKeyLength);
        if (!map) continue;

        fout(
            "String str(Icon icon)\n"
            "{\n"
            "    switch(icon) {\n"
        );

        for (int i = 0; i < map->count(); ++i)
            fout("        case Icon::%%: return \"%%\";\n") << left(map->keyAt(i), maxKeyLength) << map->keyAt(i);

        fout(
            "    }\n"
            "\n"
            "    return String{};\n"
            "}\n"
        );
    }
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->baseName();
    try {
        Ref<VariantMap> options = VariantMap::create();
        options->insert("family-info", false);
        options->insert("glyph-list", false);
        options->insert("icon-enum", false);
        options->insert("icon-str", false);
        Ref<Arguments> arguments = Arguments::parse(argc, argv, options);

        if (options->value("family-info"))
            printFamilyInfo(arguments);
        else if (options->value("glyph-list"))
            printGlyphLists(arguments);
        else if (options->value("icon-enum"))
            printIconEnum(arguments);
        else if (options->value("icon-str"))
            printIconStr(arguments);
        else
            printFamilyInfo(arguments);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]...\n"
            "Print information about font files.\n"
            "\n"
            "Options:\n"
            "  -family-info   print information about the font family (default)\n"
            "  -glyph-list    list all glyphs\n"
            "  -icon-enum     list all glyphs as a C++ enum\n"
            "  -icon-str      create a stringification method for the C++ enum\n"
        ) << toolName;
    }
    return 0;
}
