/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_BUILDSHELL_H
#define FLUXMAKE_BUILDSHELL_H

#include <flux/String>

namespace flux { class FileStatus; }

namespace fluxmake {

using namespace flux;

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
    void symlink(String path, String newPath);

    bool install(String sourcePath, String destPath);
    bool unlink(String path);

    void cd(String path);

private:
    BuildPlan *plan_;
};

} // namespace fluxmake

#endif // FLUXMAKE_BUILDSHELL_H
