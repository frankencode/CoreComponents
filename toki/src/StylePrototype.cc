/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/StylePrototype>
#include <cc/toki/Style>

namespace cc {
namespace toki {

Ref<StylePrototype> StylePrototype::create()
{
    return new StylePrototype;
}

StylePrototype::StylePrototype():
    MetaPrototype{"Style"}
{
    insert("rule", "");
    insert("ink", Color{});
    insert("paper", Color{});
    insert("bold", false);
    insert("italic", false);
}

Ref<MetaObject> StylePrototype::produce() const
{
    return Style::create();
}

}} // namespace cc::toki
