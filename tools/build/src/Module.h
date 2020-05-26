/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>

namespace ccbuild {

using namespace cc;

class Module: public Object
{
public:
    static Ref<Module> create(const string &analyseCommand, const string &modulePath, const StringList *dependencyPaths, bool dirty) {
        return new Module{analyseCommand, modulePath, dependencyPaths, dirty};
    }

    string analyseCommand() const { return analyseCommand_; }
    string modulePath() const { return modulePath_; }
    string sourcePath() const { return dependencyPaths_->at(0); }
    string toolName() const { return sourcePath()->baseName(); }
    const StringList *dependencyPaths() const { return dependencyPaths_; }
    bool dirty() const { return dirty_; }

private:
    Module(const string &analyseCommand, const string &modulePath, const StringList *dependencyPaths, bool dirty):
        analyseCommand_{analyseCommand},
        modulePath_{modulePath},
        dependencyPaths_{dependencyPaths},
        dirty_{dirty}
    {}

    string analyseCommand_;
    string modulePath_;
    Ref<const StringList> dependencyPaths_;
    bool dirty_;
};

typedef List< Ref<Module> > ModuleList;

} // namespace ccbuild
