/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_PREDICATE_H
#define FLUXMAKE_PREDICATE_H

#include <flux/MetaObject>
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
    inline String command() const { return command_; }

private:
    Predicate(MetaObject *object):
        source_(object->value("source")),
        target_(object->value("target")),
        command_(object->value("command"))
    {}

    Ref<StringList> source_;
    String target_;
    String command_;
};

typedef List< Ref<Predicate> > PredicateList;

} // namespace fluxmake

#endif // FLUXMAKE_PREDICATE_H
