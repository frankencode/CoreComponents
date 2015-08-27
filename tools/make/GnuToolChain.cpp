/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Pattern>
#include <flux/File>
#include <flux/Process>
#include <flux/ProcessFactory>
#include "BuildPlan.h"
#include "CwdGuard.h"
#include "GnuToolChain.h"

namespace fluxmake {

Ref<GnuToolChain> GnuToolChain::create(String compiler)
{
    if (compiler == "") compiler = Process::env("CC");
    if (compiler == "") compiler = "gcc";
    return new GnuToolChain(compiler);
}

GnuToolChain::GnuToolChain(String compiler):
    ToolChain(compiler, queryMachine(compiler)),
    dependencySplitPattern_("{1..:[\\:\\\\\n\r ]}")
{}

String GnuToolChain::queryMachine(String compiler)
{
    String machine = Process::start(machineCommand(compiler), Process::ForwardOutput)->readAll();
    machine->trimInsitu();
    machine->replaceInsitu("-pc-", "-"); // workaround for clang/bash
    return machine;
}

String GnuToolChain::machineCommand(String compiler)
{
    return compiler + " -dumpmachine";
}

String GnuToolChain::machineCommand() const
{
    return machineCommand(compiler());
}

String GnuToolChain::defaultOptimization(BuildPlan *plan) const
{
    /*if ((plan->options() & BuildPlan::Debug) && (plan->options() & BuildPlan::Release)) return "g";*/
    if (plan->options() & BuildPlan::Release) return "2";
    else return "";
}

String GnuToolChain::analyseCommand(BuildPlan *plan, String source) const
{
    Format args;
    args << compiler();
    appendCompileOptions(args, plan);
    args << "-MM" << source;
    return args->join(" ");
}

Ref<Job> GnuToolChain::createAnalyseJob(BuildPlan *plan, String source)
{
    return Job::create(analyseCommand(plan, source));
}

Ref<Module> GnuToolChain::finishAnalyseJob(BuildPlan *plan, Job *job)
{
    Ref<StringList> parts = job->outputText()->split(dependencySplitPattern_);
    return Module::create(job->command(), plan->modulePath(parts->pop(0)), parts, true);
}

Ref<Job> GnuToolChain::createCompileJob(BuildPlan *plan, Module *module)
{
    Format args;
    args << compiler();
    appendCompileOptions(args, plan);
    args << "-c" << "-o" << module->modulePath();
    args << module->sourcePath();
    String command = args->join(" ");
    return Job::create(command);
}

Ref<Job> GnuToolChain::createLinkJob(BuildPlan *plan, Module *module)
{
    Format args;
    args << compiler();
    if (plan->linkStatic()) args << "-static";
    args << "-pthread";
    args << "-o" << module->toolName();
    args << module->modulePath();
    appendLinkOptions(args, plan);
    String command = args->join(" ");
    return Job::create(command);
}

String GnuToolChain::linkName(BuildPlan *plan) const
{
    String name;
    if (plan->options() & BuildPlan::Library)
        name = "lib" + plan->name() + ".so." + plan->version();
    else
        name = plan->name();
    return name;
}

bool GnuToolChain::link(BuildPlan *plan)
{
    String name = plan->name();
    String version = plan->version();
    int options = plan->options();
    ModuleList *modules = plan->modules();

    Format args;

    args << compiler();
    if (plan->linkStatic()) args << "-static";
    if ((options & BuildPlan::Library) && !plan->linkStatic()) args << "-shared";
    args << "-pthread";

    if (options & BuildPlan::Library) {
        Ref<StringList> versions = version->split(".");
        args << "-Wl,-soname,lib" + name + ".so." + versions->at(0);
    }

    args << "-o" << linkName(plan);

    for (int i = 0; i < modules->count(); ++i)
        args << modules->at(i)->modulePath();

    appendLinkOptions(args, plan);

    String command = args->join(" ");

    if (!plan->shell()->run(command))
        return false;

    if ((options & BuildPlan::Library) && !plan->linkStatic())
        createLibrarySymlinks(plan, linkName(plan));

    if (options & BuildPlan::Application)
        createAliasSymlinks(plan, linkName(plan));

    return true;
}

bool GnuToolChain::includeTest(BuildPlan *plan, String includePath, StringList *testIncludes) const
{
    if (testIncludes->count() == 0) return true;

    String srcPath = File::createUnique("/tmp/XXXXXXXX.cpp");
    Ref<File> src = File::open(srcPath, File::WriteOnly);
    src->unlinkWhenDone();
    {
        Format format;
        for (int i = 0; i < testIncludes->count(); ++i)
            format << "#include <" << testIncludes->at(i) << ">" << nl;
        format << "int main() { return 0; }" << nl;
        src->write(format->join());
    }
    src->close();
    Format args;
    args << compiler();
    appendCompileOptions(args, plan);
    args << src->path() << "-I" + includePath;
    args << "-o" + src->path() + "_";
    appendLinkOptions(args, plan);

    String command = args->join(" ");
    if (plan->options() & BuildPlan::Verbose)
        fout() << "# " << command << nl;
    return Process::start(command)->wait() == 0;
}

bool GnuToolChain::linkTest(BuildPlan *plan, String libraryPath, StringList *testLibraries) const
{
    if (testLibraries->count() == 0) return true;

    String srcPath = File::createUnique("/tmp/XXXXXXXX.cpp");
    Ref<File> src = File::open(srcPath, File::WriteOnly);
    src->unlinkWhenDone();
    src->write("int main() { return 0; }\n");
    src->close();
    Format args;
    args << compiler() << src->path() << "-L" + libraryPath;
    for (int i = 0; i < testLibraries->count(); ++i) args << "-l" + testLibraries->at(i);
    args << "-o" + src->path() + "_";

    String command = args->join(" ");
    if (plan->options() & BuildPlan::Verbose)
        fout() << "# " << command << nl;
    return Process::start(command)->wait() == 0;
}

bool GnuToolChain::testHeaderPath(BuildPlan *plan, String headerPath) const
{
    String srcPath = File::createUnique("/tmp/XXXXXXXX.cpp");
    Ref<File> src = File::open(srcPath, File::WriteOnly);
    src->unlinkWhenDone();
    {
        Format format;
        format << "#include <" << headerPath << ">\n";
        format << "int main() { return 0; }\n";
        src->write(format->join());
    }
    src->close();

    Format args;
    args << compiler() << "-M";
    String command = args->join(" ");
    if (plan->options() & BuildPlan::Verbose)
        fout() << "# " << command << nl;

    return Process::start(command, Process::ForwardOutput) == 0;
}

bool GnuToolChain::install(BuildPlan *plan)
{
    int options = plan->options();
    String product = linkName(plan);
    String installDirPath = plan->installPath((options & BuildPlan::Library) ? "lib" : "bin");
    String installFilePath = installDirPath->expandPath(product);

    if (!plan->shell()->install(product, installFilePath)) return false;

    if ((options & BuildPlan::Library) && !plan->linkStatic()) {
        CwdGuard guard(installDirPath, plan->shell());
        createLibrarySymlinks(plan, product);
    }

    if (options & BuildPlan::Application && plan->alias()->count() > 0) {
        CwdGuard guard(installDirPath, plan->shell());
        createAliasSymlinks(plan, product);
    }

    if (plan->bundle()->count() > 0) {
        for (int i = 0; i < plan->bundle()->count(); ++i) {
            String path = plan->bundle()->at(i);
            String relativePath = path->copy(plan->projectPath()->count(), path->count());
            plan->shell()->install(path, plan->installRoot()->expandPath(bundlePrefix(plan)->expandPath(relativePath)));
        }
    }

    return true;
}

bool GnuToolChain::install(BuildPlan *plan, Module *module)
{
    String product = module->toolName();
    return plan->shell()->install(product, plan->installPath("bin")->expandPath(product));
}

bool GnuToolChain::uninstall(BuildPlan *plan)
{
    int options = plan->options();
    String product = linkName(plan);
    String installDirPath = plan->installPath((options & BuildPlan::Library) ? "lib" : "bin");
    String installFilePath = installDirPath->expandPath(product);

    try {
        plan->shell()->unlink(installFilePath);
    }
    catch (SystemError &) {
        return false;
    }

    if ((options & BuildPlan::Library) && !plan->linkStatic()) {
        CwdGuard guard(installDirPath, plan->shell());
        cleanLibrarySymlinks(plan, product);
    }

    if (options & BuildPlan::Application && plan->alias()->count() > 0) {
        CwdGuard guard(installDirPath, plan->shell());
        cleanAliasSymlinks(plan, product);
    }

    return false;
}

bool GnuToolChain::uninstall(BuildPlan *plan, Module *module)
{
    return plan->shell()->unlink(plan->installPath("bin/" + module->toolName()));
}

void GnuToolChain::clean(BuildPlan *plan)
{
    for (int i = 0; i < plan->modules()->count(); ++i) {
        plan->shell()->unlink(plan->modules()->at(i)->modulePath());
        if (plan->options() & BuildPlan::Tools)
            plan->shell()->unlink(plan->modules()->at(i)->toolName());
    }

    String product = linkName(plan);
    plan->shell()->unlink(product);

    if ((plan->options() & BuildPlan::Library) && !plan->linkStatic())
        cleanLibrarySymlinks(plan, product);

    if (plan->options() & BuildPlan::Application)
        cleanAliasSymlinks(plan, product);
}

String GnuToolChain::bundlePrefix(BuildPlan *plan)
{
    String name;
    if (plan->options() & BuildPlan::Library)
        name = "lib" + plan->name();
    else
        name = plan->name();
    return plan->installPrefix()->expandPath("share")->expandPath(name);
}

void GnuToolChain::appendCompileOptions(Format args, BuildPlan *plan)
{
    if (plan->options() & BuildPlan::Debug) args << "-g";
    if (plan->options() & BuildPlan::Release) args << "-DNDEBUG";
    if (plan->optimize() != "") args << "-O" + plan->optimize();
    if (plan->linkStatic()) args << "-static";
    if (plan->options() & BuildPlan::Library) args << "-fPIC";
    else args << "-fPIE";
    args << "-Wall" << "-pthread" << "-pipe";
    if (plan->bundle()->count() > 0)
        args << "-DFLUXMAKE_BUNDLE_PREFIX=" + bundlePrefix(plan);
    if (plan->name() != "")
        args << "-DFLUXMAKE_BUNDLE_VERSION=" + plan->version();
    if (plan->customCompileFlags()) {
        for (int i = 0; i < plan->customCompileFlags()->count(); ++i)
            args << plan->customCompileFlags()->at(i);
    }
    for (int i = 0; i < plan->includePaths()->count(); ++i)
        args << "-I" + plan->includePaths()->at(i);
}

void GnuToolChain::appendLinkOptions(Format args, BuildPlan *plan)
{
    if (plan->customLinkFlags()) {
        for (int i = 0; i < plan->customLinkFlags()->count(); ++i)
            args << plan->customLinkFlags()->at(i);
    }

    StringList *libraryPaths = plan->libraryPaths();
    StringList *libraries = plan->libraries();

    for (int i = 0; i < libraryPaths->count(); ++i)
        args << "-L" + libraryPaths->at(i);

    for (int i = 0; i < libraries->count(); ++i)
        args << "-l" + libraries->at(i);

    if (plan->containsCPlusPlus()) args << "-lstdc++";

    if (libraryPaths->count() > 0 || plan->installPrefix() != "/usr") {
        Ref<StringList> rpaths = StringList::create();
        if (plan->installPrefix() != "/usr")
            *rpaths << "-rpath=" + plan->installPrefix()->expandPath("lib");
        for (int i = 0; i < libraryPaths->count(); ++i)
            *rpaths << "-rpath=" + libraryPaths->at(i)->absolutePathRelativeTo(Process::cwd());
        args << "-Wl,--enable-new-dtags," + rpaths->join(",");
    }
}

void GnuToolChain::createLibrarySymlinks(BuildPlan *plan, String libName)
{
    cleanLibrarySymlinks(plan, libName);

    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->symlink(libName, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(BuildPlan *plan, String libName)
{
    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->unlink(parts->join("."));
}

void GnuToolChain::createAliasSymlinks(BuildPlan *plan, String appName)
{
    cleanAliasSymlinks(plan, appName);

    for (int i = 0; i < plan->alias()->count(); ++i)
        plan->shell()->symlink(appName, plan->alias()->at(i));
}

void GnuToolChain::cleanAliasSymlinks(BuildPlan *plan, String appName)
{
    for (int i = 0; i < plan->alias()->count(); ++i)
        plan->shell()->unlink(plan->alias()->at(i));
}

} // namespace fluxmake
