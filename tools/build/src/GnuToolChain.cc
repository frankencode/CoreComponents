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
#include <cc/Dir>
#include <cc/glob/Pattern>
#include "BuildPlan.h"
#include "CwdGuard.h"
#include "LinkJob.h"
#include "GnuToolChain.h"

namespace ccbuild {

Ref<GnuToolChain> GnuToolChain::create(const BuildPlan *plan)
{
    return new GnuToolChain(plan);
}

GnuToolChain::GnuToolChain(const BuildPlan *plan):
    dependencySplitPattern_("{1..:[\\:\\\\\n\r ]}"),
    rpathOverride_(Process::env("CCBUILD_RPATH_OVERRIDE"))
{
    if (plan->compiler() != "") {
        ccPath_ = plan->compiler();
        cxxPath_ = plan->compiler();
    }
    else {
        ccPath_ = Process::env("CC");
        cxxPath_ = Process::env("CXX");
    }

    if (ccPath_ == "") ccPath_ = "gcc";
    if (cxxPath_ == "") cxxPath_ = "g++";

    Process::setEnv("CC", ccPath_);
    Process::setEnv("CXX", cxxPath_);

    machine_ = queryMachine(ccPath_);
    systemRoot_ = querySystemRoot(ccPath_);

    isMultiArch_ = Dir::exists(plan->installPath("lib")->extendPath(machine_));
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

String GnuToolChain::compiler(const BuildPlan *plan) const
{
    return plan->containsCPlusPlus() ? cxxPath_ : ccPath_;
}

String GnuToolChain::queryMachine(String compiler)
{
    String machine = SubProcess::open(machineCommand(compiler), stdErr())->readAll();
    mutate(machine)->trimInsitu();
    mutate(machine)->replaceInsitu("-pc-", "-"); // workaround for clang/bash
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
    mutate(systemRoot)->trimInsitu();
    return systemRoot;
}

String GnuToolChain::defaultOptimization(const BuildPlan *plan) const
{
    /*if ((plan->options() & BuildPlan::Debug) && (plan->options() & BuildPlan::Release)) return "g";*/
    if (plan->options() & BuildPlan::Release) return "2";
    else return "";
}

String GnuToolChain::analyseCommand(const BuildPlan *plan, String source) const
{
    Format args;
    args << compiler(source);
    appendCompileOptions(args, plan);
    args << "-MM" << source;
    return args->join(" ");
}

Ref<Job> GnuToolChain::createAnalyseJob(const BuildPlan *plan, String source) const
{
    return Job::create(analyseCommand(plan, source));
}

Ref<Module> GnuToolChain::finishAnalyseJob(const BuildPlan *plan, const Job *job) const
{
    Ref<StringList> parts = dependencySplitPattern_->split(job->outputText());
    String modulePath = parts->pop(0);
    if (plan->options() & BuildPlan::Tools)
        modulePath = modulePath->baseName();
    else
        modulePath = plan->modulePath(modulePath);
    return Module::create(job->command(), modulePath, parts, true);
}

Ref<Job> GnuToolChain::createCompileJob(const BuildPlan *plan, const Module *module) const
{
    Format args;
    args << compiler(module->sourcePath());
    args << "-c" << "-o" << module->modulePath();
    appendCompileOptions(args, plan);
    args << module->sourcePath();
    String command = args->join(" ");
    return Job::create(command);
}

Ref<Job> GnuToolChain::createCompileLinkJob(const BuildPlan *plan, const Module *module) const
{
    Format args;
    args << compiler(module->sourcePath());
    args << "-o" << module->toolName();
    appendCompileOptions(args, plan);
    if (plan->linkStatic()) args << "-static";
    args << "-pthread";
    args << module->sourcePath();
    appendLinkOptions(args, plan);
    String command = args->join(" ");
    return Job::create(command);
}

String GnuToolChain::targetName(const BuildPlan *plan) const
{
    String name = plan->name();
    if (plan->options() & BuildPlan::Library) {
        if (!plan->name()->beginsWith("lib"))
            name = "lib" + name;
    }
    return name;
}

String GnuToolChain::linkName(const BuildPlan *plan) const
{
    String name = targetName(plan);
    if (plan->options() & BuildPlan::Library)
        name = name + ".so." + plan->version();
    return name;
}

String GnuToolChain::linkCommand(const BuildPlan *plan) const
{
    String name = plan->name();
    Version version = plan->version();
    int options = plan->options();
    ModuleList *modules = plan->modules();

    Format args;

    args << compiler(plan);
    args << "-o" << linkName(plan);

    if (plan->linkStatic()) args << "-static";
    if ((options & BuildPlan::Library) && !plan->linkStatic()) args << "-shared";
    args << "-pthread";

    if (options & BuildPlan::Library) {
        args << (
            Format() << "-Wl,-soname,lib" << name << ".so." << version->major()
        )->join();
    }

    Ref<StringList> modulePaths = StringList::create();
    for (const Module *module: modules)
        modulePaths << module->modulePath();
    args->appendList(modulePaths->sort());

    appendLinkOptions(args, plan);

    return args->join(" ");
}

Ref<Job> GnuToolChain::createLinkJob(const BuildPlan *plan) const
{
    return LinkJob::create(plan);
}

bool GnuToolChain::link(const BuildPlan *plan) const
{
    if (!plan->shell()->run(linkCommand(plan)))
        return false;

    return createSymlinks(plan);
}

bool GnuToolChain::testInclude(const BuildPlan *plan, const StringList *headers) const
{
    String srcPath = File::createUnique("/tmp/########.cc");
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
        auto stage = SubProcess::stage();
        stage->setCommand(command);
        auto overloads = SubProcess::Overloads::create();
        {
            Ref<File> devNull = File::open("/dev/null", OpenMode::WriteOnly);
            overloads->insert(StandardOutputFd, devNull);
            if (!(plan->options() & BuildPlan::Verbose))
                overloads->insert(StandardErrorFd, devNull);
        }
        stage->setOverloads(overloads);
        sub = stage->open();
    }

    return sub->wait() == 0;
}

String GnuToolChain::configureCompileCommand(const BuildPlan *plan, String sourcePath, String binPath) const
{
    Format args;
    args << compiler(sourcePath);
    args << "-o" << binPath;
    // appendCompileOptions(args, plan);
    args << "-pthread";
    args << sourcePath;
    // appendLinkOptions(args, plan);
    return args->join(" ");
}

String GnuToolChain::installDirPath(const BuildPlan *plan) const
{
    String relativePath;
    if (plan->options() & BuildPlan::Library) {
        relativePath = "lib";
        if (isMultiArch_)
            relativePath->extendPath(machine_);
    }
    else {
        relativePath = "bin";
    }
    return plan->installPath(relativePath);
}

String GnuToolChain::includePrefix(const BuildPlan *plan) const
{
    return plan->installPath("include");
}

String GnuToolChain::libIncludePrefix(const BuildPlan *plan) const
{
    return installDirPath(plan)->extendPath(linkName(plan))->extendPath("include");
}

String GnuToolChain::bundlePrefix(const BuildPlan *plan) const
{
    return plan->installPrefix()->extendPath("share")->extendPath(targetName(plan));
}

bool GnuToolChain::createSymlinks(const BuildPlan *plan) const
{
    if ((plan->options() & BuildPlan::Library) && !plan->linkStatic())
        createLibrarySymlinks(plan, linkName(plan));

    if (plan->options() & BuildPlan::Plugin)
        createPluginSymlinks(plan, targetName(plan->extensionTarget()), linkName(plan));

    if (plan->options() & BuildPlan::Application)
        createAliasSymlinks(plan, linkName(plan));

    return true;
}

void GnuToolChain::createLibrarySymlinks(const BuildPlan *plan, String libName) const
{
    cleanLibrarySymlinks(plan, libName);

    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->symlink(libName, parts->join("."));
}

void GnuToolChain::cleanLibrarySymlinks(const BuildPlan *plan, String libName) const
{
    Ref<StringList> parts = libName->split('.');
    while (parts->popBack() != "so")
        plan->shell()->unlink(parts->join("."));
}

void GnuToolChain::createPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const
{
    cleanPluginSymlinks(plan, targetLibName, pluginLibName);

    String pluginPath = targetLibName->extendPath("plugins");
    plan->shell()->mkdir(pluginPath);
    plan->shell()->symlink("../../" + pluginLibName, pluginPath->extendPath(pluginLibName));
}

void GnuToolChain::cleanPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const
{
    String pluginPath = targetLibName->extendPath("plugins");
    plan->shell()->clean(pluginPath->extendPath(pluginLibName));
}

void GnuToolChain::cleanPluginSymlinks(const BuildPlan *plan, String targetLibName) const
{
    String pluginPath = targetLibName->extendPath("plugins");
    plan->shell()->clean(pluginPath);
}

void GnuToolChain::createAliasSymlinks(const BuildPlan *plan, String appName) const
{
    cleanAliasSymlinks(plan, appName);

    for (String aliasName: plan->alias())
        plan->shell()->symlink(appName, aliasName);
}

void GnuToolChain::cleanAliasSymlinks(const BuildPlan *plan, String appName) const
{
    for (String aliasName: plan->alias())
        plan->shell()->unlink(aliasName);
}

String GnuToolChain::pkgConfigName(const BuildPlan *plan) const
{
    return targetName(plan) + ".pc";
}

String GnuToolChain::pkgConfigInstallDirPath(const BuildPlan *plan) const
{
    return installDirPath(plan)->extendPath("pkgconfig");
}

String GnuToolChain::pkgConfig(const BuildPlan *plan) const
{
    if (!(plan->options() & BuildPlan::Library)) return "";

    bool hasLibInclude = Dir::exists(plan->projectPath()->extendPath("libinclude"));
    Format f;
    f << "prefix=" << plan->installPrefix() << nl;
    f << "exec_prefix=${prefix}" << nl;
    f << "libdir=" << installDirPath(plan) << nl;
    f << "includedir=${prefix}/include" << nl;
    if (hasLibInclude)
        f << "libincludedir=${libdir}/" << targetName(plan) << "/include" << nl;
    f << nl;

    f << "Name: " << targetName(plan) << nl;
    f << "Description: " << plan->description() << nl;
    f << "Version: " << plan->version() << nl;
    f << "Libs: -L${libdir} -l" << plan->name() << nl;

    if (plan->prerequisites()->count() > 0) {
        auto requiresList = List<const BuildPlan *>::create();
        for (const BuildPlan *prerequisite: plan->prerequisites()) {
            if (prerequisite->options() & BuildPlan::Package) {
                for (const BuildPlan *child: prerequisite->prerequisites()) {
                    if (child->options() & BuildPlan::Library)
                        requiresList->append(child);
                }
            }
            else if (prerequisite->options() & BuildPlan::Library) {
                requiresList->append(prerequisite);
            }
        }
        if (requiresList->count() > 0) {
            f << "Requires:";
            for (const BuildPlan *prerequisite: requiresList) {
                f << " " << targetName(prerequisite);
            }
            f << nl;
        }
    }

    if (plan->systemPrerequisitesByName()) {
        bool gotPkgConfigPrerequisites = false;
        for (int i = 0; i < plan->systemPrerequisitesByName()->count(); ++i) {
            const SystemPrerequisiteList *prerequisiteList = plan->systemPrerequisitesByName()->valueAt(i);
            for (const SystemPrerequisite *prerequisite: prerequisiteList) {
                if (prerequisite->autoConfigure()) {
                    if (!gotPkgConfigPrerequisites) {
                        gotPkgConfigPrerequisites = true;
                        f << "Requires.private: ";
                    }
                    else {
                        f << " ";
                    }
                    f << prerequisite->name();
                }
            }
        }
        if (gotPkgConfigPrerequisites) f << nl;
    }

    f << "Cflags: -I${includedir}";
    if (hasLibInclude)
        f << " -I${libincludedir}";
    f << nl;
    return f;
}

void GnuToolChain::generatePkgConfig(const BuildPlan *plan) const
{
    File::save(pkgConfigName(plan), pkgConfig(plan));
}

bool GnuToolChain::refreshLinkerCache(const BuildPlan *plan) const
{
    if (plan->installRoot() != "/") return true;
    String libInstallPath = plan->installPath("lib");
    if (isMultiArch_) libInstallPath = libInstallPath->extendPath(machine_);
    if (!libInstallPath->isAbsolutePath()) libInstallPath = libInstallPath->absolutePathRelativeTo(Process::cwd());
    Format attr;
    attr << "ldconfig";
    attr << libInstallPath;
    String command = attr->join(" ");
    return plan->shell()->run(command);
}

void GnuToolChain::appendCompileOptions(Format args, const BuildPlan *plan) const
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

void GnuToolChain::appendLinkOptions(Format args, const BuildPlan *plan) const
{
    if (plan->customLinkFlags()) {
        for (int i = 0; i < plan->customLinkFlags()->count(); ++i)
            args << plan->customLinkFlags()->at(i);
    }

    //if (plan->options() & BuildPlan::Plugin)
    //    args << "-Wl,--no-as-needed";

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
