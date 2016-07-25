/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/crypto/Sha1>
#include <cc/crypto/Md5>
#include <cc/crypto/HashMeter>

using namespace cc;
using namespace cc::crypto;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();
    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        arguments->validate(VariantMap::create());

        StringList *items = arguments->items();
        if (items->count() == 0) items->append("");

        for (int i = 0; i < items->count(); ++i) {
            String path = items->at(i);
            Ref<HashSum> hashSum;
            if (toolName->contains("sha1")) hashSum = Sha1::create();
            else hashSum = Md5::create();
            Ref<HashMeter> hashMeter = HashMeter::open(hashSum);
            Ref<Stream> source;
            if (path != "") source = File::open(path);
            else { source = cc::stdIn(); path = "-"; }
            source->transferTo(hashMeter);
            fout() << hashMeter->finish()->toHex() << "\t" << path << nl;
        }
    }
    catch (HelpError &) {
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
