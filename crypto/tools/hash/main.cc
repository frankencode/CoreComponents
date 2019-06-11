/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/File>
#include <cc/Transfer>
#include <cc/crypto/Sha1Sink>
#include <cc/crypto/Md5Sink>
#include <cc/crypto/HashMeter>

using namespace cc;
using namespace cc::crypto;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();
    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        arguments->validate(VariantMap::create());

        Ref<const StringList> items = arguments->items();
        if (items->count() == 0) items = StringList::create() << "";

        for (String path: items) {
            Ref<HashSink> hashSink;
            if (toolName->contains("sha1")) hashSink = Sha1Sink::open();
            else hashSink = Md5Sink::open();
            Ref<Stream> source;
            if (path != "") source = File::open(path);
            else { source = cc::stdIn(); path = "-"; }
            source->transferTo(hashSink);
            fout() << hashSink->finish()->toHex() << "\t" << path << nl;
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]...\n"
            "Computes %% sums of files.\n"
        ) << toolName << (toolName->contains("sha1") ? "SHA1" : "MD5");
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }
    return 0;
}
