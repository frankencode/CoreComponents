/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LineSource>
#include <cc/stdio>

int main()
{
    using namespace cc;

    List<String> list;
    for (const String &line: LineSource { stdInput() }) {
        list.append(line);
    }

    list.sort();

    for (const String &line: list) {
        fout() << line << nl;
    }

    return 0;
}
