/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildShell>
#include <cc/Process>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/stdio>

namespace cc::build {

String BuildShell::beautify(const String &command) const
{
    if (plan().options() & BuildOption::Bootstrap) {
        return command
            .replaced(plan().sourcePrefix(), "$SOURCE")
            .replaced(Process::cwd(), "$PWD")
            .replaced("$ORIGIN", "'$ORIGIN'");
    }
    return command;
}

bool BuildShell::run(const String &command) const
{
    fout() << beautify(command) << nl;
    if (plan().options() & BuildOption::Simulate) return true;
    return Process{command}.wait() == 0;
}

FileStatus BuildShell::fileStatus(const String &path) const
{
    if (plan().options() & BuildOption::Blindfold) return FileStatus{};
    return FileStatus{path, false};
}

void BuildShell::mkdir(const String &path) const
{
    if (!fileStatus(path))
        fout("mkdir -p %%\n") << path;
    if (plan().options() & BuildOption::Simulate) return;
    Dir::establish(path);
}

void BuildShell::rmdir(const String &path) const
{
    bool exists = fileStatus(path).isValid();
    if (exists) fout("rmdir %%\n") << path;
    if (plan().options() & BuildOption::Simulate) return;
    if (exists) try { Dir::remove(path); } catch (SystemError &) { /*FIXME, directory might not be empty */ }
}

bool BuildShell::clean(const String &path) const
{
    fout("rm -rf %%\n") << path;
    if (plan().options() & BuildOption::Simulate) return true;

    if (fileStatus(path)) {
        try {
            File::clean(path);
        }
        catch (SystemError &error) {
            ferr() << error << nl;
            return false;
        }
    }

    return true;
}

void BuildShell::symlink(const String &path, const String &newPath) const
{
    fout("ln -sf %% %%\n") << path << newPath;
    if (plan().options() & BuildOption::Simulate) return;
    try { File::unlink(newPath); } catch (SystemError &) {}
    File::symlink(path, newPath);
}

bool BuildShell::install(const String &sourcePath, const String &destPath) const
{
    String destDirPath = destPath.cdUp();
    bool destDirMissing = destDirPath != "" && !fileStatus(destDirPath);
    if (destDirMissing) fout("install -d %%\n") << destDirPath;

    fout("install %% %%\n") << sourcePath << destPath;

    if (plan().options() & BuildOption::Simulate) return true;

    try {
        if (destDirMissing) Dir::establish(destDirPath);
        File source{sourcePath};
        FileStatus sourceStatus{sourcePath};
        File sink{destPath, FileOpen::WriteOnly|FileOpen::Create|FileOpen::Truncate, sourceStatus.mode()};
        sink.write(source.map());
    }
    catch (SystemError &error) {
        fout("%%\n") << error;
        return false;
    }

    return true;
}

bool BuildShell::unlink(const String &path) const
{
    if ((plan().options() & BuildOption::Blindfold) || fileStatus(path)) {
        if (plan().options() & BuildOption::Simulate) {
            fout("rm -f %%\n") << path;
            return true;
        }
        fout("rm %%\n") << path;
        try {
            File::unlink(path);
        }
        catch (SystemError &error) {
            fout("%%\n") << error;
            return false;
        }
    }
    return true;
}

bool BuildShell::installAll(const String &sourcePrefix, const String &installPrefix) const
{
    DirWalker walker;
    try { walker = DirWalker{sourcePrefix}; }
    catch (...) { return true; }

    try {
        String sourcePath;
        bool isDir = false;
        while (walker.read(&sourcePath, &isDir)) {
            if (isDir) continue;
            if (
                !install(
                    sourcePath,
                    installPrefix / sourcePath.copy(sourcePrefix.count() + 1, sourcePath.count())
                )
            )
                return false;
        }
    }
    catch (SystemError &error) {
        fout("%%\n") << error;
        return false;
    }

    return true;
}

bool BuildShell::unlinkAll(const String &sourcePrefix, const String &installPrefix) const
{
    DirWalker walker;
    try { walker = DirWalker{sourcePrefix}; }
    catch (...) { return true; }

    try {
        String sourcePath;
        bool isDir = false;
        while (walker.read(&sourcePath, &isDir)) {
            if (isDir) continue;
            if (!
                clean(
                    installPrefix / sourcePath.copy(sourcePrefix.count() + 1, sourcePath.count())
                )
            )
                return false;
        }
    }
    catch (SystemError &error) {
        fout("%%\n") << error;
        return false;
    }

    return true;
}

void BuildShell::cd(const String &path) const
{
    fout("cd %%\n") << path;
    if (plan().options() & BuildOption::Simulate) return;
    try { Process::cd(path); } catch (SystemError &) { /*FIXME*/ }
}

} // namespace cc::build
