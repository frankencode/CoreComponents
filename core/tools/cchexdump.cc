#include <cc/stdio>
#include <cc/File>
#include <cc/Arguments>
#include <cc/HexDump>
#include <cc/exceptions>

using namespace cc;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        arguments->validate(VariantMap::create());

        Ref<const StringList> items = arguments->items();
        if (items->count() == 0) items = StringList::create() << "";

        for (String path: items)
        {
            Ref<Stream> source;
            if (path == "") source = stdIn();
            else source = File::open(path);

            Ref<HexDump> hexDump = HexDump::open(stdOut());
            source->transferTo(hexDump);
        }
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [FILE]...\n"
            "Hexdump files (or stdin if no file is presented).\n"
        ) << toolName;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }

    return 0;
}
