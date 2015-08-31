/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_PREDICATE_H
#define FLUXMAKE_PREDICATE_H

#include <flux/meta/MetaObject>
#include <flux/List>

namespace fluxmake {

using namespace flux;

class Predicate: public MetaObject
{
public:
    inline static Ref<Predicate> read(MetaObject *object) {
        return new Predicate(object);
    }

    inline StringList *source() const { return source_; }
    inline String target() const { return target_; }
    inline String create() const { return create_; }
    inline String update() const { return update_; }
    inline String remove() const { return remove_; }

private:
    Predicate(MetaObject *object):
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

} // namespace fluxmake

#endif // FLUXMAKE_PREDICATE_H
