/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha3HashSink>
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
            if (toolName.contains("sha3-")) hashSink = Sha3HashSink<512>{};
            else if (toolName.contains("sha3-384")) hashSink = Sha3HashSink<384>{};
            else if (toolName.contains("sha3-256")) hashSink = Sha3HashSink<256>{};
            else if (toolName.contains("sha3-224")) hashSink = Sha3HashSink<224>{};
            else if (toolName.contains("sha2-512")) hashSink = Sha512HashSink{};
            else if (toolName.contains("sha2-256")) hashSink = Sha256HashSink{};
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
        if (toolName.contains("sha3-512")) hashName = "SHA3-512";
        else if (toolName.contains("sha3-384")) hashName = "SHA3-384";
        else if (toolName.contains("sha3-256")) hashName = "SHA3-256";
        else if (toolName.contains("sha3-224")) hashName = "SHA3-224";
        else if (toolName.contains("sha2-512")) hashName = "SHA2-512";
        else if (toolName.contains("sha2-256")) hashName = "SHA2-256";
        else if (toolName.contains("sha1")) hashName = "SHA1";
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
