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
    static Ref<Predicate> read(const MetaObject *object) {
        return new Predicate(object);
    }

    StringList *source() const { return source_; }
    string target() const { return target_; }
    string create() const { return create_; }
    string update() const { return update_; }
    string remove() const { return remove_; }
    string clean() const { return clean_; }

private:
    Predicate(const MetaObject *object):
        source_(object->value("source")),
        target_(object->value("target")),
        create_(object->value("create")),
        update_(object->value("update")),
        remove_(object->value("remove")),
        clean_(object->value("clean"))
    {}

    Ref<StringList> source_;
    string target_;
    string create_;
    string update_;
    string remove_;
    string clean_;
};

typedef List< Ref<Predicate> > PredicateList;

} // namespace ccbuild
