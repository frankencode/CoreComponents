/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
