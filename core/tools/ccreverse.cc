/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include <cc/File>

using namespace cc;

String reverse(String v)
{
    String w = String::allocate(v->count());
    for (int i = 0, n = v->count(); i < n; ++i)
        mutate(w)->at(i) = v->at(n - i - 1);
    return w;
}

int main(int argc, char **argv)
{
    try {
        if (argc > 1) {
            for (int i = 1; i < argc; ++i)
                stdOut()->write(reverse(File::open(argv[i])->map()));
        }
        else {
            stdOut()->write(reverse(stdIn()->readAll()));
        }
    }
    catch (ConnectionResetByPeer &)
    {}

    return 0;
}
