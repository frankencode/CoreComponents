/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_MODULE_H
#define FLUXMAKE_MODULE_H

#include <flux/String>

namespace fluxmake {

using namespace flux;

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
    Module(String analyseCommand, String modulePath, StringList *dependencyPaths, bool dirty)
        : analyseCommand_(analyseCommand),
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

} // namespace fluxmake

#endif // FLUXMAKE_MODULE_H
