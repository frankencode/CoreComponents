/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXKIT_STRINGTRAP_H
#define FLUXKIT_STRINGTRAP_H

#include <string.h>
#include <flux/assert>
#include <flux/String>

namespace flux {
namespace abnf {

class StringTrap: public Object // FIXME: can easily be replaced by Queue<String>
{
public:
    inline static Ref<StringTrap> create() {
        return new StringTrap;
    }

    inline const char *capture(ByteArray *s) const {
        return capture(s->chars(), s->count());
    }

    inline const char *capture(ByteArray *s, int i0, int i1) const {
        FLUX_ASSERT((0 <= i0) && (i0 < s->count()));
        FLUX_ASSERT((0 <= i1) && (i1 <= s->count()));
        FLUX_ASSERT(i0 <= i1);
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

}} // namespace flux::abnf

#endif // FLUXKIT_STRINGTRAP_H
