/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/build/CodyMessage>
#include <cc/build/CodyMessageSyntax>

namespace cc::build {

CodyMessage CodyMessage::read(const String &line)
{
    return CodyMessage { CodyMessageSyntax{}.read(line) };
}

} // namespace cc::build
