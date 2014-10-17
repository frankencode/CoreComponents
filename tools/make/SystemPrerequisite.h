/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_SYSTEMPREREQUISITE_H
#define FLUXMAKE_SYSTEMPREREQUISITE_H

#include "BuildParameters.h"

namespace fluxmake
{

using namespace flux;

class BuildPlan;

class SystemPrerequisite: public BuildParameters
{
public:
    static Ref<SystemPrerequisite> read(MetaObject *object, BuildPlan *plan);

    inline String name() const { return name_; }
    inline String value() const { return value_; }
    inline String description() const { return description_; }
    inline bool optional() const { return optional_; }

    inline StringList *testIncludes() const { return testIncludes_; }
    inline StringList *testLibraries() const { return testLibraries_; }

private:
    SystemPrerequisite(MetaObject *object, BuildPlan *plan);

    String name_;
    String value_;
    String description_;
    bool optional_;

    Ref<StringList> testIncludes_;
    Ref<StringList> testLibraries_;
};

typedef List< Ref<SystemPrerequisite> > SystemPrerequisiteList;
typedef Map< String, Ref<SystemPrerequisiteList> > SystemPrerequisitesByName;

} // namespace fluxmake

#endif // FLUXMAKE_SYSTEMPREREQUISITE_H
