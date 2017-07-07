/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>
#include <cc/meta/MetaProtocol>

namespace ccbuild {

using namespace cc;
using namespace cc::meta;

class BuildPlan;

class BuildParameters: public Object
{
public:
    inline static Ref<BuildParameters> create() { return new BuildParameters; }
    void read(MetaObject *object, BuildPlan *plan = 0);
    void readSpecific(MetaObject *object);
    void readSpecific(BuildParameters *specific);

    inline String compiler() const { return compiler_; }
    inline String optimize() const { return optimize_; }
    inline bool linkStatic() const { return linkStatic_; }

    inline StringList *includePaths() const { return includePaths_; }
    inline StringList *libraryPaths() const { return libraryPaths_; }
    inline StringList *libraries() const { return libraries_; }

    inline StringList *customCompileFlags() const { return customCompileFlags_; }
    inline StringList *customLinkFlags() const { return customLinkFlags_; }

protected:
    BuildParameters() {}

    String compiler_;
    String optimize_;
    bool linkStatic_;

    Ref<StringList> includePaths_;
    Ref<StringList> libraryPaths_;
    Ref<StringList> libraries_;

    Ref<StringList> customCompileFlags_;
    Ref<StringList> customLinkFlags_;
};

} // namespace ccbuild

