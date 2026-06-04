/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/CsvFormat>
#include <cc/stdio>

namespace cc {

CsvFormat::~CsvFormat()
{
    String line = join();
    if (!sink_) stdOutput().write(line);
    else sink_.write(line);
}

} // namespace cc
