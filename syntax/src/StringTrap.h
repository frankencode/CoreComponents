/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <string.h>
#include <cc/assert>
#include <cc/String>

namespace cc {
namespace syntax {

class StringTrap: public Object // FIXME: can easily be replaced by Queue<String>
{
public:
    inline static Ref<StringTrap> create() {
        return new StringTrap;
    }

    inline const char *capture(const ByteArray *s) const {
        return capture(s->chars(), s->count());
    }

    inline const char *capture(const ByteArray *s, int i0, int i1) const {
        CC_ASSERT((0 <= i0) && (i0 < s->count()));
        CC_ASSERT((0 <= i1) && (i1 <= s->count()));
        CC_ASSERT(i0 <= i1);
        return capture(s->chars() + i0, i1 - i0);
    }

    inline const char *capture(const char *s, int len = -1) const {
        if (len < 0) len = strlen(s);
        if (!head_) head_ = tail_ = new Node(s, len);
        else tail_ = tail_->next_ = new Node(s, len);
        return tail_->s_->chars();
    }

private:
    StringTrap(): head_(0), tail_(0) {}

    class Node: public Object {
    public:
        Node(const char *s, int len): s_(ByteArray::copy(s, len)) {}
        Ref<ByteArray> s_;
        Ref<Node> next_;
    };
    mutable Ref<Node> head_;
    mutable Node *tail_;
};

}} // namespace cc::syntax
