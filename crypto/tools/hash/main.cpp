/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/exceptions>
#include <flux/meta/Arguments>
#include <flux/crypto/Sha1>
#include <flux/crypto/Md5>
#include <flux/crypto/HashMeter>

using namespace flux;
using namespace flux::crypto;

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
            Ref<File> source;
            if (path != "") source = File::open(path);
            else { source = flux::in(); path = "-"; }
            source->transferAll(hashMeter);
            fout() << hashMeter->finish()->hex() << "\t" << path << nl;
        }
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [FILE]...\n"
            "Computes %% sums of files.\n"
        ) << toolName << (toolName->contains("sha1") ? "SHA1" : "MD5");
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }
    return 0;
}
