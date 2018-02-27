/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Style>

namespace cc {
namespace toki {

Ref<MetaObject> Style::produce()
{
    return Style::create();
}

void Style::define()
{
    className("Style");
    insert("rule", "");
    insert("ink", Color());
    insert("paper", Color());
    insert("bold", false);
    insert("italic", false);
}

void Style::realize(const CharArray *text, Token *objectToken)
{
    ruleName_ = value("rule");
    ink_ = value("ink");
    paper_ = value("paper");
    bold_ = value("bold");
    italic_ = value("italic");
}

}} // namespace cc::toki
