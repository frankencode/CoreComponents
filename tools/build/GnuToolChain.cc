/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
        Ref<File> src = File::open(srcPath, File::WriteOnly);
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
            Ref<File> devNull = File::open("/dev/null", File::WriteOnly);
            overloads->insert(StdOutFd, devNull);
            if (!(plan->options() & BuildPlan::Verbose))
                overloads->insert(StdErrFd, devNull);
        }
        sub = SubProcess::open(params);
    }

    return sub->wait() == 0;
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

} // namespace ccbuild
