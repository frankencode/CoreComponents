/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>
#include <cc/List>

namespace ccbuild {

using namespace cc;

class Predicate: public MetaObject
{
public:
    inline static Ref<Predicate> read(const MetaObject *object) {
        return new Predicate(object);
    }

    inline StringList *source() const { return source_; }
    inline String target() const { return target_; }
    inline String create() const { return create_; }
    inline String update() const { return update_; }
    inline String remove() const { return remove_; }

private:
    Predicate(const MetaObject *object):
        source_(object->value("source")),
        target_(object->value("target")),
        create_(object->value("create")),
        update_(object->value("update")),
        remove_(object->value("remove"))
    {}

    Ref<StringList> source_;
    String target_;
    String create_;
    String update_;
    String remove_;
};

typedef List< Ref<Predicate> > PredicateList;

} // namespace ccbuild

