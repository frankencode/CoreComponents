/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    inline static Ref<Module> create(String analyseCommand, String modulePath, StringList *dependencyPaths, bool dirty) {
        return new Module(analyseCommand, modulePath, dependencyPaths, dirty);
    }

    inline String analyseCommand() const { return analyseCommand_; }
    inline String modulePath() const { return modulePath_; }
    inline String sourcePath() const { return dependencyPaths_->at(0); }
    inline String toolName() const { return sourcePath()->baseName(); }
    inline StringList *dependencyPaths() const { return dependencyPaths_; }
    inline bool dirty() const { return dirty_; }

private:
    Module(String analyseCommand, String modulePath, StringList *dependencyPaths, bool dirty):
        analyseCommand_(analyseCommand),
        modulePath_(modulePath),
        dependencyPaths_(dependencyPaths),
        dirty_(dirty)
    {}

    String analyseCommand_;
    String modulePath_;
    Ref<StringList> dependencyPaths_;
    bool dirty_;
};

typedef List< Ref<Module> > ModuleList;

} // namespace ccbuild

