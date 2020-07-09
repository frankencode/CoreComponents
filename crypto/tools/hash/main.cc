/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/Sha1Sink>
#include <cc/crypto/Md5Sink>
#include <cc/crypto/HashMeter>
#include <cc/File>
#include <cc/Transfer>
#include <cc/Arguments>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;
using namespace cc::crypto;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();
    try {
        StringList items = Arguments{argc, argv}->read();
        if (items->count() == 0) items->append("");

        for (String path: items) {
            HashSink hashSink;
            if (toolName->contains("sha1")) hashSink = Sha1Sink{};
            else hashSink = Md5Sink{};
            Stream source;
            if (path != "") source = File{path};
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
