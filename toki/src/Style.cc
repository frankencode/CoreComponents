/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Style>

namespace cc {
namespace toki {

Ref<Style> Style::create()
{
    return new Style;
}

Style::Style()
{}

void Style::realize(const CharArray *text, Token *objectToken)
{
    ruleName_ = value("rule");
    ink_ = value("ink");
    paper_ = value("paper");
    bold_ = value("bold");
    italic_ = value("italic");
}

}} // namespace cc::toki
