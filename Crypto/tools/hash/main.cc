/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha512HashSink>
#include <cc/Sha256HashSink>
#include <cc/Sha1HashSink>
#include <cc/Md5HashSink>
#include <cc/File>
#include <cc/Arguments>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.baseName();

    try {
        List<String> items = Arguments{argc, argv}.read();
        if (items.count() == 0) items.append("");

        for (String path: items) {
            HashSink hashSink;
            if (toolName.contains("sha512")) hashSink = Sha512HashSink{};
            else if (toolName.contains("sha256")) hashSink = Sha256HashSink{};
            else if (toolName.contains("sha1")) hashSink = Sha1HashSink{};
            else hashSink = Md5HashSink{};
            Stream source;
            if (path != "") source = File{path};
            else { source = IoStream::input(); path = "-"; }
            source.transferTo(hashSink);
            fout() << hex(hashSink.finish()) << "\t" << path << nl;
        }
    }
    catch (HelpRequest &) {
        const char *hashName = "?";
        if (toolName.contains("sha512")) hashName = "SHA-512";
        else if (toolName.contains("sha256")) hashName = "SHA-256";
        else if (toolName.contains("sha1")) hashName = "SHA-1";
        else hashName = "MD5";
        fout(
            "Usage: %% [FILE]...\n"
            "Computes %% sums of files.\n"
        ) << toolName << hashName;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }
    return 0;
}
