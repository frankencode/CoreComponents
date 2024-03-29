/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Pen>
#include <cc/Format>

namespace cc {

String Pen::toString() const
{
    if (isNull()) return "Pen{}";
    return me().toString();
}

String Pen::State::toString() const
{
    return Format{}
        << "Pen {\n"
        << "  color: " << color_ << "\n"
        << "  lineWidth: " << lineWidth_ << "\n"
        << "  lineCap: " << lineCap_ << "\n"
        << "  lineJoin: " << lineJoin_ << "\n"
        << "  miterLimit: " << miterLimit_ << "\n"
        << "  lineDash: " << lineDash_ << "\n"
        << "  lineDashOffset: " << lineDashOffset_ << "\n"
        << "}";
}

} // namespace cc
