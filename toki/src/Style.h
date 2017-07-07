/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Color>
#include <cc/meta/yason>

namespace cc {
namespace toki {

using namespace cc::meta;

class Palette;

class Style: public MetaObject
{
public:
    static Ref<Style> create() { return new Style; }

    inline String ruleName() const { return ruleName_; }
    inline int rule() const { return rule_; }
    inline Color ink() const { return ink_; }
    inline Color paper() const { return paper_; }
    inline bool bold() const { return bold_; }
    inline bool italic() const { return italic_; }

private:
    friend class Palette;

    Style() {}
    virtual Ref<MetaObject> produce();
    virtual void define();
    virtual void realize(const ByteArray *text, Token *objectToken);

    String ruleName_;
    int rule_;
    Color ink_;
    Color paper_;
    bool bold_;
    bool italic_;
};

}} // namespace cc::toki

