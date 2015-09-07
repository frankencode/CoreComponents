#include <flux/stdio>
#include <flux/File>
#include <flux/Utf16Source>
#include <flux/Utf8Sink>

using namespace flux;

void decode(Stream *in)
{
    Ref<Utf16Source> source = Utf16Source::open(in);
    Ref<Utf8Sink> sink = Utf8Sink::open(stdOut());
    for (uchar_t ch = 0; source->read(&ch);) {
        if (ch == '\r') continue;
        sink->write(ch);
    }
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        for (int i = 1; i < argc; ++i)
            decode(File::open(argv[i]));
    }
    else {
        decode(stdIn());
    }
    return 0;
}
