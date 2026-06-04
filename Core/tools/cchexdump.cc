/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/HexDump>
#include <cc/Arguments>
#include <cc/File>
#include <cc/exceptions>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    String toolName = String{argv[0]}.fileName();

    try {
        List<String> items = Arguments{argc, argv}.read();
        if (items.count() == 0) items << "";

        for (String path: items)
        {
            Stream source;
            if (path == "") source = stdInput();
            else source = File{path};
            source.transferTo(HexDump{});
        }
    }
    catch (HelpRequest &) {
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
