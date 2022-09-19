/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CsvFormat>
#include <cc/stdio>

namespace cc {

CsvFormat::~CsvFormat()
{
    String line = join();
    if (!sink_) IoStream::output().write(line);
    else sink_.write(line);
}

} // namespace cc
