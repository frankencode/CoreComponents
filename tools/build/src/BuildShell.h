/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>

namespace cc { class FileStatus; }

namespace ccbuild {

using namespace cc;

class BuildPlan;

class BuildShell: public Object
{
public:
    static Ref<BuildShell> create(const BuildPlan *plan) { return new BuildShell{plan}; }

    string beautify(const string &command) const;
    bool run(const string &command) const;

    Ref<FileStatus> fileStatus(const string &path) const;

    void mkdir(const string &path) const;
    void rmdir(const string &path) const;
    bool clean(const string &path) const;
    void symlink(const string &path, const string &newPath) const;

    bool install(const string &sourcePath, const string &destPath) const;
    bool unlink(const string &path) const;

    bool installAll(const string &sourcePrefix, const string &installPrefix) const;
    bool unlinkAll(const string &sourcePrefix, const string &installPrefix) const;

    void cd(const string &path) const;

private:
    BuildShell(const BuildPlan *plan);

    const BuildPlan *plan() const { return plan_; }

    const BuildPlan *plan_;
};

} // namespace ccbuild
