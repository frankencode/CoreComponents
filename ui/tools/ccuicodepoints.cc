#include <cc/stdio>
#include <cc/Arguments>
#include <cc/File>

using namespace cc;

String camelize(const String &name)
{
    String s = name;
    if (s->count() == 0) return s;
    mutate(s)->at(0) = toUpper(s->at(0));
    for (int i = 1; i < s->count() - 1; ++i) {
        if (mutate(s)->at(i) == '_')
            mutate(s)->at(i + 1) = toUpper(mutate(s)->at(i + 1));
    }
    return s->replace("_", "");
}

typedef Map<String, uchar_t> IconMap;

Ref<IconMap> parseFile(const String &path, int *maxNameLength)
{
    auto lines = File::open(path)->map()->split("\n");
    auto map = IconMap::create();
    *maxNameLength = 0;
    for (String line: lines) {
        auto parts = line->split(" ");
        if (parts->count() < 2) continue;
        String name = camelize(parts->at(0));
        uchar_t ch = parts->at(1)->toNumber<uchar_t, 16>();
        map->insert(name, ch);
        if (*maxNameLength < name->count())
            *maxNameLength = name->count();
    }
    return map;
}

void printEnum(const Arguments *arguments)
{
    for (String path: arguments->items())
    {
        int maxNameLength = 0;
        auto map = parseFile(path, &maxNameLength);

        fout(
            "enum class Icon: uchar_t {\n"
        );

        for (int i = 0; i < map->count(); ++i)
            fout() << "    " << left(map->keyAt(i), maxNameLength) << " = 0x" << hex(map->valueAt(i)) << (i != map->count() - 1 ? ",\n" : "\n");

        fout(
            "};\n"
        );
    }
}

void printStr(const Arguments *arguments)
{
    for (String path: arguments->items())
    {
        int maxNameLength = 0;
        auto map = parseFile(path, &maxNameLength);

        fout(
            "String str(Icon icon)\n"
            "{\n"
            "    switch(icon) {\n"
        );

        for (int i = 0; i < map->count(); ++i)
            fout() << "        case Icon::" << left(map->keyAt(i), maxNameLength) << ": return \"" << map->keyAt(i) << "\";\n";

        fout(
            "    }\n"
            "\n"
            "    return String{};\n"
            "};\n"
        );
    }
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->baseName();
    try {
        Ref<VariantMap> options = VariantMap::create();
        options->insert("print-enum", false);
        options->insert("print-str", false);
        Ref<Arguments> arguments = Arguments::parse(argc, argv, options);

        if (options->value("print-enum"))
            printEnum(arguments);
        else if (options->value("print-str"))
            printStr(arguments);
        else
            printEnum(arguments);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]\n"
            "Translate a codepoints file into a C++ enum\n"
            "\n"
            "Options:\n"
            "  -print-enum   print C++ enum class\n"
            "  -print-str    print C++ enum to string conversion routine\n"
            "\n"
        ) << toolName;
    }
    return 0;
}
