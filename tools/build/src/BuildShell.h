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
    static Ref<BuildShell> create(const BuildPlan *plan) { return new BuildShell(plan); }

    String beautify(String command) const;
    bool run(String command) const;

    Ref<FileStatus> fileStatus(String path) const;

    void mkdir(String path) const;
    void rmdir(String path) const;
    bool clean(String path) const;
    void symlink(String path, String newPath) const;

    bool install(String sourcePath, String destPath) const;
    bool unlink(String path) const;

    bool installAll(String sourcePrefix, String installPrefix) const;
    bool unlinkAll(String sourcePrefix, String installPrefix) const;

    void cd(String path) const;

private:
    BuildShell(const BuildPlan *plan);

    inline const BuildPlan *plan() const { return plan_; }

    const BuildPlan *plan_;
};

} // namespace ccbuild
