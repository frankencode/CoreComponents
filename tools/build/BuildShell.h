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

class BuildShell // FIXME: should inherit from Object
{
public:
    BuildShell(BuildPlan *plan);
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

    void cd(String path);

private:
    BuildPlan *plan_;
};

} // namespace ccbuild
