/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TextError>
#include <cc/ResourceContext>
#include <cc/Format>
#include <cc/input>

namespace cc {

TextError::TextError(const String &text, long offset, const String &hint):
    source_{ResourceContext{}.top()},
    text_{text},
    offset_{offset},
    hint_{hint}
{}

String TextError::message() const
{
    if (text_.contains('\n')) {
        long line = 0, pos = 0;
        if (offsetToLinePos(text_, offset_, &line, &pos)) {
            return (Format{"%%:%%:%%: %%"} << source_ << line << pos << hint_).join<String>();
        }
    }
    return Format{"%%...: %%"} << text_.copy(0, offset_) << hint_;
}

} // namespace cc
