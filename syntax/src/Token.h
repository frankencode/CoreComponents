/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Tree>

namespace cc {
namespace syntax {

class RuleNode;
class InvokeNode;
class KeywordNode;

class TokenFactory;
class Token;

/** \brief Token tree projection target
  */
class TokenScreen {
public:
    virtual bool project(Token *token, int i0, int i1) = 0;
};

/** \brief Token tree
  * \see SyntaxState
  */
class Token: public Tree<Token>
{
    friend class TokenFactory;
    friend class syntax::RuleNode;
    friend class syntax::InvokeNode;
    friend class syntax::KeywordNode;

public:
    inline bool valid() const { return rule_ != -1; }
    inline int scope() const { return scope_; }
    inline int rule() const { return rule_; }
    inline int keyword() const { return keyword_; }

    inline int i0() const { return i0_; }
    inline int i1() const { return i1_; }

    bool project(TokenScreen *screen);

    Token *at(int i) const;

protected:
    Token(int scope = 0, int rule = 0)
        : scope_(scope),
          rule_(rule),
          keyword_(-1),
          i0_(0),
          i1_(0)
    {}

private:
    inline void setRange(int i0, int i1) {
        i0_ = i0;
        i1_ = i1;
    }

    inline void setKeyword(int value) { keyword_ = value; }

    int scope_;
    int rule_;
    int keyword_;
    int i0_;
    int i1_;
};

}} // namespace cc::syntax

