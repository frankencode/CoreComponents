/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc { class FileStatus; }

namespace ccbuild {

using namespace cc;

class BuildPlan;

class BuildShell: public Object
{
public:
    static Ref<BuildShell> create(const BuildPlan *plan) { return new BuildShell{plan}; }

    String beautify(const String &command) const;
    bool run(const String &command) const;

    Ref<FileStatus> fileStatus(const String &path) const;

    void mkdir(const String &path) const;
    void rmdir(const String &path) const;
    bool clean(const String &path) const;
    void symlink(const String &path, const String &newPath) const;

    bool install(const String &sourcePath, const String &destPath) const;
    bool unlink(const String &path) const;

    bool installAll(const String &sourcePrefix, const String &installPrefix) const;
    bool unlinkAll(const String &sourcePrefix, const String &installPrefix) const;

    void cd(const String &path) const;

private:
    BuildShell(const BuildPlan *plan);

    const BuildPlan *plan() const { return plan_; }

    const BuildPlan *plan_;
};

} // namespace ccbuild
