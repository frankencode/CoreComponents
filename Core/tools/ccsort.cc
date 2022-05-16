/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>
#include <cc/stdio>

int main()
{
    using namespace cc;

    List<String> list;
    LineSource source{IoStream::input()};
    for (const String &line: source)
        list.append(line);

    list.sort();

    for (const String &line: list)
        fout() << line << nl;

    return 0;
}
