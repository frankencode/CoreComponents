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
    static Ref<BuildParameters> create() { return new BuildParameters; }
    void read(const MetaObject *object, const BuildPlan *plan = nullptr);
    void readSpecific(const MetaObject *object);
    void readSpecific(BuildParameters *specific);

    String compiler() const { return compiler_; }
    String optimize() const { return optimize_; }
    bool linkStatic() const { return linkStatic_; }

    StringList includePaths() const { return includePaths_; }
    StringList libraryPaths() const { return libraryPaths_; }
    StringList libraries() const { return libraries_; }
    StringList customCompileFlags() const { return customCompileFlags_; }
    StringList customLinkFlags() const { return customLinkFlags_; }

    StringList &includePaths() { return includePaths_; }
    StringList &libraryPaths() { return libraryPaths_; }
    StringList &libraries() { return libraries_; }
    StringList &customCompileFlags() { return customCompileFlags_; }
    StringList &customLinkFlags() { return customLinkFlags_; }

protected:
    BuildParameters() {}

    String compiler_;
    String optimize_;
    bool linkStatic_;

    StringList includePaths_;
    StringList libraryPaths_;
    StringList libraries_;

    StringList customCompileFlags_;
    StringList customLinkFlags_;
};

} // namespace ccbuild
