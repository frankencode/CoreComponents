#include <cc/Arguments>
#include <cc/File>
#include <cc/stdio>

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

using IconMap = MapValue<String, uchar_t>;

IconMap parseFile(const String &path, int *maxNameLength)
{
    auto lines = File::open(path)->map()->split("\n");
    IconMap map;
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

void printEnum(const StringList &items)
{
    for (String path: items)
    {
        int maxNameLength = 0;
        auto map = parseFile(path, &maxNameLength);

        fout(
            "enum class Icon: uchar_t {\n"
        );

        for (int i = 0; i < map->count(); ++i)
            fout() << "    " << left(map->at(i)->key(), maxNameLength) << " = 0x" << hex(map->at(i)->value()) << (i != map->count() - 1 ? ",\n" : "\n");

        fout(
            "};\n"
        );
    }
}

void printStr(const StringList &items)
{
    for (String path: items)
    {
        int maxNameLength = 0;
        auto map = parseFile(path, &maxNameLength);

        fout(
            "String str(Icon icon)\n"
            "{\n"
            "    switch(icon) {\n"
        );

        for (int i = 0; i < map->count(); ++i)
            fout() << "        case Icon::" << left(map->at(i)->key(), maxNameLength) << ": return \"" << map->at(i)->key() << "\";\n";

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
        VariantMap options;
        options->insert("print-enum", false);
        options->insert("print-str", false);

        StringList items = Arguments{argc, argv}->read(options);

        if (options->value("print-enum"))
            printEnum(items);
        else if (options->value("print-str"))
            printStr(items);
        else
            printEnum(items);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]\n"
            "Translate a codepoints file into a C++ enum\n"
            "\n"
            "Options:\n"
            "  -print-enum   print C++ enum class\n"
            "  -print-str    print C++ enum to String conversion routine\n"
            "\n"
        ) << toolName;
    }
    return 0;
}
