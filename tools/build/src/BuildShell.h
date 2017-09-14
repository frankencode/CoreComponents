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
    static Ref<BuildShell> create(BuildPlan *plan) { return new BuildShell(plan); }

    inline BuildPlan *plan() const { return plan_; }

    String beautify(String command);
    bool run(String command);

    Ref<FileStatus> fileStatus(String path);

    void mkdir(String path);
    void rmdir(String path);
    bool clean(String path);
    void symlink(String path, String newPath);

    bool install(String sourcePath, String destPath);
    bool unlink(String path);

    bool installAll(String sourcePrefix, String installPrefix);
    bool unlinkAll(String sourcePrefix, String installPrefix);

    void cd(String path);

private:
    BuildShell(BuildPlan *plan);

    BuildPlan *plan_;
};

} // namespace ccbuild
