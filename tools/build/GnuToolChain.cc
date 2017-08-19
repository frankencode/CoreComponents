/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/UnlinkGuard>
#include <cc/Process>
#include <cc/SubProcess>
#include <cc/regexp/RegExp>
#include "BuildPlan.h"
#include "CwdGuard.h"
#include "GnuToolChain.h"

namespace ccbuild {

Ref<GnuToolChain> GnuToolChain::create(String compiler)
{
    return new GnuToolChain(compiler);
}

GnuToolChain::GnuToolChain(String compiler):
    dependencySplitPattern_("{1..:[\\:\\\\\n\r ]}"),
    rpathOverride_(Process::env("CCBUILD_RPATH_OVERRIDE"))
{
    if (compiler != "") {
        ccPath_ = compiler;
        cxxPath_ = compiler;
    }
    else {
        ccPath_ = Process::env("CC");
        cxxPath_ = Process::env("CXX");
    }

    if (ccPath_ == "") ccPath_ = "gcc";
    if (cxxPath_ == "") cxxPath_ = "g++";

    machine_ = queryMachine(ccPath_);
    systemRoot_ = querySystemRoot(ccPath_);
}

GnuToolChain::~GnuToolChain()
{}

String GnuToolChain::compiler(String source) const
{
    return (
        source->endsWith(".cc")  ||
        source->endsWith(".cxx") ||
        source->endsWith(".cpp") ||
        source->endsWith(".c++") ||
        source->endsWith(".mm")  ||
        source->endsWith(".M")   ||
        source->endsWith(".C")   ||
        source->endsWith(".cp")  ||
        source->endsWith(".CPP")
    ) ?
    cxxPath_ :
    ccPath_;
}

String GnuToolChain::compiler(BuildPlan *plan) const
{
    return plan->containsCPlusPlus() ? cxxPath_ : ccPath_;
}

String GnuToolChain::queryMachine(String compiler)
{
    String machine = SubProcess::open(machineCommand(compiler), stdErr())->readAll();
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

String GnuToolChain::querySystemRoot(String compiler)
{
    String systemRoot = SubProcess::open(compiler + " -print-sysroot", stdErr())->readAll();
    systemRoot->trimInsitu();
    return systemRoot;
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
    args << compiler(source);
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
    Ref<StringList> parts = dependencySplitPattern_->split(job->outputText());
    return Module::create(job->command(), plan->modulePath(parts->pop(0)), parts, true);
}

Ref<Job> GnuToolChain::createCompileJob(BuildPlan *plan, Module *module)
{
    Format args;
    args << compiler(module->sourcePath());
    appendCompileOptions(args, plan);
    args << "-c" << "-o" << module->modulePath();
    args << module->sourcePath();
    String command = args->join(" ");
    return Job::create(command);
}

Ref<Job> GnuToolChain::createLinkJob(BuildPlan *plan, Module *module)
{
    Format args;
    args << compiler(module->sourcePath());
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

String GnuToolChain::linkCommand(BuildPlan *plan) const
{
    String name = plan->name();
    Version version = plan->version();
    int options = plan->options();
    ModuleList *modules = plan->modules();

    Format args;

    args << compiler(plan);
    if (plan->linkStatic()) args << "-static";
    if ((options & BuildPlan::Library) && !plan->linkStatic()) args << "-shared";
    args << "-pthread";

    if (options & BuildPlan::Library) {
        args << (
            Format() << "-Wl,-soname,lib" << name << ".so." << Version::major(version)
        )->join();
    }

    args << "-o" << linkName(plan);

    Ref<StringList> modulePaths = StringList::create();
    for (int i = 0; i < modules->count(); ++i)
        modulePaths << modules->at(i)->modulePath();
    args->appendList(modulePaths->sort());

    appendLinkOptions(args, plan);

    return args->join(" ");
}

bool GnuToolChain::link(BuildPlan *plan)
{
    int options = plan->options();
    String command = linkCommand(plan);

    if (!plan->shell()->run(command))
        return false;

    if ((options & BuildPlan::Library) && !plan->linkStatic())
        createLibrarySymlinks(plan, linkName(plan));

    if (options & BuildPlan::Application)
        createAliasSymlinks(plan, linkName(plan));

    return true;
}

bool GnuToolChain::testInclude(BuildPlan *plan, StringList *headers) const
{
    String srcPath = File::createUnique("/tmp/XXXXXXXX.cc");
    UnlinkGuard srcUnlinkGuard(srcPath);
    UnlinkGuard objUnlinkGuard(srcPath->baseName() + ".o");
    {
        Ref<File> src = File::open(srcPath, OpenMode::WriteOnly);
        Format format;
        for (int i = 0; i < headers->count(); ++i)
            format << "#include <" << headers->at(i) << ">\n";
        format << "int main() { return 0; }\n";
        src->write(format->join());
    }

    Format args;
    args << compiler();
    appendCompileOptions(args, plan);
    args << "-E" << srcPath;
    String command = args->join(" ");
    if (plan->options() & BuildPlan::Verbose)
        fout() << "# " << command << nl;

    Ref<SubProcess> sub;
    {
        Ref<SubProcess::Params> params = SubProcess::Params::create();
        params->setCommand(command);
        Ref<SubProcess::Overloads> overloads = SubProcess::Overloads::create();
        {
            Ref<File> devNull = File::open("/dev/null", OpenMode::WriteOnly);
            overloads->insert(StandardOutputFd, devNull);
            if (!(plan->options() & BuildPlan::Verbose))
                overloads->insert(StandardErrorFd, devNull);
        }
        sub = SubProcess::open(params);
    }

    return sub->wait() == 0;
}

String GnuToolChain::installDirPath(BuildPlan *plan) const
{
    return plan->installPath((plan->options() & BuildPlan::Library) ? "lib" : "bin");
}

String GnuToolChain::bundlePrefix(BuildPlan *plan) const
{
    String name;
    if (plan->options() & BuildPlan::Library)
        name = "lib" + plan->name();
    else
        name = plan->name();
    return plan->installPrefix()->expandPath("share")->expandPath(name);
}

void GnuToolChain::createLibrarySymlinks(BuildPlan *plan, String libName) const
{
    cleanLibrarySymlinks(plan, libName);

    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->symlink(libName, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(BuildPlan *plan, String libName) const
{
    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->unlink(parts->join("."));
}

void GnuToolChain::createAliasSymlinks(BuildPlan *plan, String appName) const
{
    cleanAliasSymlinks(plan, appName);

    for (String aliasName: plan->alias())
        plan->shell()->symlink(appName, aliasName);
}

void GnuToolChain::cleanAliasSymlinks(BuildPlan *plan, String appName) const
{
    for (String aliasName: plan->alias())
        plan->shell()->unlink(aliasName);
}

void GnuToolChain::appendCompileOptions(Format args, BuildPlan *plan) const
{
    if (plan->options() & BuildPlan::Debug) args << "-g";
    if (plan->options() & BuildPlan::Release) args << "-DNDEBUG";
    if (plan->optimize() != "") args << "-O" + plan->optimize();
    if (plan->linkStatic()) args << "-static";
    if (!(
        plan->customCompileFlags()->contains("-fPIC") ||
        plan->customCompileFlags()->contains("-fPIE") ||
        plan->customCompileFlags()->contains("-fpie") ||
        plan->customCompileFlags()->contains("-fpic")
    )) {
        /*if (plan->options() & BuildPlan::Library) args << "-fPIC";
        else*/ args << "-fPIC";
    }
    args << "-Wall" << "-pthread" << "-pipe" << "-D_FILE_OFFSET_BITS=64";
    if (plan->bundle()->count() > 0)
        args << "-DCCBUILD_BUNDLE_PREFIX=" + bundlePrefix(plan);
    if (plan->name() != "")
        args << "-DCCBUILD_BUNDLE_VERSION=" + plan->version();
    if (plan->customCompileFlags()) {
        for (int i = 0; i < plan->customCompileFlags()->count(); ++i)
            args << plan->customCompileFlags()->at(i);
    }
    for (int i = 0; i < plan->includePaths()->count(); ++i)
        args << "-I" + plan->includePaths()->at(i);
}

void GnuToolChain::appendLinkOptions(Format args, BuildPlan *plan) const
{
    if (plan->customLinkFlags()) {
        for (int i = 0; i < plan->customLinkFlags()->count(); ++i)
            args << plan->customLinkFlags()->at(i);
    }

    args << "-Wl,--no-as-needed";

    StringList *libraryPaths = plan->libraryPaths();
    StringList *libraries = plan->libraries();

    if (!libraryPaths->contains("."))
        args << "-L.";

    for (int i = 0; i < libraryPaths->count(); ++i)
        args << "-L" + libraryPaths->at(i);

    for (int i = 0; i < libraries->count(); ++i)
        args << "-l" + libraries->at(i);

    if (plan->containsCPlusPlus() && !cxxPath_->contains("++")) args << "-lstdc++";

    Ref<StringList> rpaths = StringList::create();
    if (rpathOverride_ != "")
        rpaths << "-rpath=" + rpathOverride_;
    else
        rpaths << "-rpath=$ORIGIN" << "-rpath=$ORIGIN/../lib";

    if (libraryPaths->count() > 0) {
        for (int i = 0; i < libraryPaths->count(); ++i)
            rpaths << "-rpath=" + libraryPaths->at(i)->absolutePathRelativeTo(Process::cwd());
    }

    rpaths << "-rpath-link=" + Process::cwd();
    args << "-Wl,--enable-new-dtags," + rpaths->join(",");
}

} // namespace ccbuild
