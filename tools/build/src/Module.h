/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace ccbuild {

using namespace cc;

class Module: public Object
{
public:
    static Ref<Module> create(const String &analyseCommand, const String &modulePath, const StringList *dependencyPaths, bool dirty) {
        return new Module{analyseCommand, modulePath, dependencyPaths, dirty};
    }

    String analyseCommand() const { return analyseCommand_; }
    String modulePath() const { return modulePath_; }
    String sourcePath() const { return dependencyPaths_->at(0); }
    String toolName() const { return sourcePath()->baseName(); }
    const StringList *dependencyPaths() const { return dependencyPaths_; }
    bool dirty() const { return dirty_; }

private:
    Module(const String &analyseCommand, const String &modulePath, const StringList *dependencyPaths, bool dirty):
        analyseCommand_{analyseCommand},
        modulePath_{modulePath},
        dependencyPaths_{dependencyPaths},
        dirty_{dirty}
    {}

    String analyseCommand_;
    String modulePath_;
    Ref<const StringList> dependencyPaths_;
    bool dirty_;
};

typedef List< Ref<Module> > ModuleList;

} // namespace ccbuild
