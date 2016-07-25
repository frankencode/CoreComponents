/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>
#include <cc/meta/MetaObject>
#include <cc/toki/Style>

namespace cc {
namespace toki {

using namespace yason;

class Palette: public MetaObject
{
public:
    static Ref<Palette> create() { return new Palette; }
    static Ref<Palette> load(String path);

    inline String scopeName() const { return scopeName_; }
    inline int scope() const { return scope_; }

    static int defaultScope();

    enum DefaultRule {
        Text,
        CurrentLine,
        Cursor,
        Selection,
        Match,
        LineNumber,
        CurrentLineNumber,
        Undefined = -1
    };

    inline Style *styleByRule(int rule) { return styleByRule_->value(rule); }

    inline int styleCount() const { return styleByRule_->count(); }
    inline const Style *styleAt(int i) const { return styleByRule_->valueAt(i); }

private:
    Palette();

    static int defaultRuleByName(String name);

    virtual Ref<MetaObject> produce();
    virtual void define();
    virtual void realize(const ByteArray *text, Token *objectToken);

    String scopeName_;
    int scope_;

    typedef Map<int, Ref<Style> > StyleByRule;
    Ref<StyleByRule> styleByRule_;
};

}} // namespace cc::toki

