/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/GnuToolChain>
#include <cc/build/LinkJob>
#include <cc/Pattern>
#include <cc/Format>
#include <cc/Process>
#include <cc/Command>
#include <cc/File>
#include <cc/Dir>

namespace cc::build {

struct GnuToolChain::State: public ToolChain::State
{
    State(const String &compiler, const String &libInstallPath):
        rpathOverride_{Process::env("CCBUILD_RPATH_OVERRIDE")}
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

        #if defined __CYGWIN32__ || defined __CYGWIN__
        cygwin_ = true;
        #else
        cygwin_ = cxxPath_.contains("cygwin");
        #endif

        Process::setEnv("CC", ccPath_);
        Process::setEnv("CXX", cxxPath_);

        machine_ = queryMachine(ccPath_);
        systemRoot_ = querySystemRoot(ccPath_);

        isMultiArch_ = Dir::exists(libInstallPath / machine_);

        cFlags_ = Process::env("CFLAGS");
        cxxFlags_ = Process::env("CXXFLAGS");
        lFlags_ = Process::env("LFLAGS");
    }

    static String queryMachine(const String &compiler)
    {
        String machine = Process{machineCommand(compiler)}.output().readAll();
        machine.trim();
        machine.replace("-pc-", "-"); // workaround for clang/bash
        return machine;
    }

    static String machineCommand(const String &compiler)
    {
        return compiler + " -dumpmachine";
    }

    static String querySystemRoot(const String &compiler)
    {
        if (compiler.contains("clang")) return String{}; //! \todo add support for sysroot detection with clang

        String text =
            Process{
                Command{compiler + " -print-sysroot"}
                .io(2, IoStream::error())
            }.output().readAll();

        text.trim();

        return text;
    }

    String machine() const override
    {
        return machine_;
    }

    String systemRoot() const override
    {
        return systemRoot_;
    }

    String compiler(const String &source = "") const override
    {
        return (
            source.endsWith(".cc")  ||
            source.endsWith(".cxx") ||
            source.endsWith(".cpp") ||
            source.endsWith(".c++") ||
            source.endsWith(".mm")  ||
            source.endsWith(".M")   ||
            source.endsWith(".C")   ||
            source.endsWith(".cp")  ||
            source.endsWith(".CPP")
        ) ?
        cxxPath_ :
        ccPath_;
    }

    String compiler(const BuildPlan &plan) const override
    {
        return plan.containsCPlusPlus() ? cxxPath_ : ccPath_;
    }

    String machineCommand() const override
    {
        return machineCommand(compiler());
    }

    String defaultOptimization(BuildOption options) const override
    {
        /*if ((options & BuildOption::Debug) && (options & BuildOption::Release)) return "g";*/
        if (options & BuildOption::Release) return "2";
        else return "";
    }

    String analyseCommand(const BuildPlan &plan, const String &source) const override
    {
        Format args;
        args << compiler(source);
        appendCompileOptions(args, plan);
        args << "-MM" << source;
        return args.join<String>(' ');
    }

    Job createAnalyseJob(const BuildPlan &plan, const String &source) const override
    {
        return Job{analyseCommand(plan, source)};
    }

    Module finishAnalyseJob(const BuildPlan &plan, const Job &job) const override
    {
        List<String> parts = dependencySplitPattern_.breakUp(job.outputText());
        String modulePath = parts.first();
        parts.popFront();
        if (plan.options() & BuildOption::Tools) {
            modulePath = modulePath.baseName();
            if (cygwin_) modulePath = modulePath + ".exe";
        }
        else {
            modulePath = plan.modulePath(modulePath);
        }
        return Module{job.command(), modulePath, parts, true};
    }

    Job createCompileJob(const BuildPlan &plan, const Module &module) const override
    {
        Format args;
        args << compiler(module.sourcePath());
        args << "-c" << "-o" << module.modulePath();
        appendCompileOptions(args, plan);
        args << module.sourcePath();
        return Job{args.join<String>(' ')};
    }

    Job createCompileLinkJob(const BuildPlan &plan, const Module &module) const override
    {
        Format args;
        args << compiler(module.sourcePath());
        args << "-o" << linkName(module);
        appendCompileOptions(args, plan);
        if (plan.linkStatic()) args << "-static";
        args << "-pthread";
        args << module.sourcePath();
        appendLinkOptions(args, plan);
        return Job{args.join<String>(' ')};
    }

    Job createPreprocessJob(const BuildPlan &plan, const Module &module) const override
    {
        Format args;
        args << compiler(module.sourcePath());
        args << "-E";
        appendCompileOptions(args, plan);
        args << module.sourcePath();
        return Job{args.join<String>(' ')};
    }

    String targetName(const BuildPlan &plan) const override
    {
        String name = plan.name();

        if (plan.options() & BuildOption::Library) {
            if (cygwin_);
            else {
                if (!plan.name().startsWith("lib")) {
                    name = "lib" + name;
                }
            }
        }
        return name;
    }

    String linkName(const BuildPlan &plan) const override
    {
        String name = targetName(plan);

        if (plan.options() & BuildOption::Library) {
            if (cygwin_) {
                name = "cyg" + name + ".dll";
            }
            else {
                name = name + ".so." + str(plan.version());
            }
        }
        else if (
            plan.options() & BuildOption::Application ||
            plan.options() & BuildOption::Test
        ) {
            if (cygwin_) {
                name = name + ".exe";
            }
        }

        return name;
    }

    String linkName(const Module &module) const override
    {
        String name = module.sourcePath().baseName();

        if (cygwin_) {
            name = name + ".exe";
        }

        return name;
    }

    String linkCommand(const BuildPlan &plan) const override
    {
        String name = plan.name();
        Version version = plan.version();
        List<Module> modules = plan.modules();

        Format args;

        args << compiler(plan);
        args << "-o" << linkName(plan);

        if (plan.linkStatic()) args << "-static";
        if ((plan.options() & BuildOption::Library) && !plan.linkStatic()) args << "-shared";
        args << "-pthread";

        if (plan.options() & BuildOption::Library) {
            args << (
                Format{} << "-Wl,-soname,lib" << name << ".so." << version.major()
            ).join<String>();
        }

        List<String> modulePaths;
        for (const Module &module: modules)
            modulePaths << module.modulePath();
        args.appendList(modulePaths.sorted());

        appendLinkOptions(args, plan);

        return args.join<String>(' ');
    }

    Job createLinkJob(const BuildPlan &plan) const override
    {
        return LinkJob{plan};
    }

    bool link(const BuildPlan &plan) const override
    {
        if (!plan.shell().run(linkCommand(plan)))
            return false;

        return createSymlinks(plan);
    }

    String configureCompileCommand(const BuildPlan &plan, const String &sourcePath, const String &binPath) const override
    {
        Format args;
        args << compiler(sourcePath);
        args << "-o" << binPath;
        // appendCompileOptions(args, plan);
        args << "-DNDEBUG" << "-pthread";
        args << sourcePath;
        // appendLinkOptions(args, plan);
        return args.join<String>(' ');
    }

    String relativeInstallDirPath(const BuildPlan &plan) const
    {
        String relativePath;
        if (plan.options() & BuildOption::Library) {
            if (cygwin_) {
                relativePath = "bin";
            }
            else {
                relativePath = "lib";
                if (isMultiArch_) {
                    relativePath = relativePath / machine_;
                }
            }
        }
        else {
            relativePath = "bin";
        }
        return relativePath;
    }

    String installDirPath(const BuildPlan &plan) const override
    {
        return plan.installPath(relativeInstallDirPath(plan));
    }

    String installedDirPath(const BuildPlan &plan) const override
    {
        return plan.installedPath(relativeInstallDirPath(plan));
    }

    String includePrefix(const BuildPlan &plan) const override
    {
        return plan.installPath("include");
    }

    String libIncludePrefix(const BuildPlan &plan) const override
    {
        return installDirPath(plan) / linkName(plan) / "include";
    }

    String bundlePrefix(const BuildPlan &plan) const override
    {
        return plan.installPrefix() / "share" / targetName(plan);
    }

    bool createSymlinks(const BuildPlan &plan) const override
    {
        if ((plan.options() & BuildOption::Library) && !plan.linkStatic()) {
            createLibrarySymlinks(plan, linkName(plan));
        }
        if (plan.options() & BuildOption::Plugin) {
            createPluginSymlinks(plan, linkName(plan.extensionTarget()).baseName(), linkName(plan));
        }
        if (plan.options() & BuildOption::Application) {
            createAliasSymlinks(plan, linkName(plan));
        }
        return true;
    }

    void createLibrarySymlinks(const BuildPlan &plan, const String &libName) const override
    {
        if (cygwin_) return;

        cleanLibrarySymlinks(plan, libName);

        List<String> parts = libName.split('.');
        while (parts.last() != "so") {
            parts.popBack();
            plan.shell().symlink(libName, parts.join('.'));
        }
    }

    void cleanLibrarySymlinks(const BuildPlan &plan, const String &libName) const override
    {
        if (cygwin_) return;

        List<String> parts = libName.split('.');
        while (parts.last() != "so") {
            parts.popBack();
            plan.shell().unlink(parts.join('.'));
        }
    }

    void createPluginSymlinks(const BuildPlan &plan, const String &targetLibName, const String &pluginLibName) const override
    {
        cleanPluginSymlinks(plan, targetLibName, pluginLibName);

        String pluginPath = plan.pluginPath(targetLibName);
        plan.shell().mkdir(pluginPath);
        plan.shell().symlink(plan.pluginReversePath() / pluginLibName, pluginPath / pluginLibName);
    }

    void cleanPluginSymlinks(const BuildPlan &plan, const String &targetLibName, const String &pluginLibName) const override
    {
        plan.shell().clean(plan.pluginPath(targetLibName) / pluginLibName);
    }

    void cleanPluginSymlinks(const BuildPlan &plan, const String &targetLibName) const override
    {
        plan.shell().clean(plan.pluginPath(targetLibName));
    }

    void createAliasSymlinks(const BuildPlan &plan, const String &appName) const override
    {
        cleanAliasSymlinks(plan, appName);

        for (const String &aliasName: plan.alias())
            plan.shell().symlink(appName, aliasName);
    }

    void cleanAliasSymlinks(const BuildPlan &plan, const String &appName) const override
    {
        for (const String &aliasName: plan.alias())
            plan.shell().unlink(aliasName);
    }

    String pkgConfigName(const BuildPlan &plan) const override
    {
        return targetName(plan) + ".pc";
    }

    String pkgConfigInstallDirPath(const BuildPlan &plan) const override
    {
        return installDirPath(plan) / "pkgconfig";
    }

    String pkgConfig(const BuildPlan &plan) const override
    {
        if (!(plan.options() & BuildOption::Library)) return "";

        bool hasLibInclude = Dir::exists(plan.projectPath() / "libinclude");
        Format f;
        f << "prefix=" << plan.installPrefix() << nl;
        f << "exec_prefix=${prefix}" << nl;
        f << "libdir=" << installedDirPath(plan) << nl;
        f << "includedir=${prefix}/include" << nl;
        if (hasLibInclude)
            f << "libincludedir=${libdir}/" << targetName(plan) << "/include" << nl;
        f << nl;

        f << "Name: " << targetName(plan) << nl;
        f << "Description: " << plan.description() << nl;
        f << "Version: " << plan.version() << nl;
        f << "Libs: -L${libdir} -l" << plan.name() << nl;

        {
            List<String> requiresList;

            for (const BuildPlan &prerequisite: plan.prerequisites()) {
                if (prerequisite.options() & BuildOption::Package) {
                    for (const BuildPlan &child: prerequisite.prerequisites()) {
                        if (child.options() & BuildOption::Library)
                            requiresList.append(targetName(child));
                    }
                }
                else if (prerequisite.options() & BuildOption::Library) {
                    requiresList.append(targetName(prerequisite));
                }
            };

            if (requiresList.count() > 0) {
                f << "Requires: " << requiresList.join(' ') << nl;
            }
        }

        {
            List<String> requiresList;

            for (const auto &item: plan.systemPrerequisitesByName()) {
                const SystemPrerequisite &prerequisite = item.value();
                if (prerequisite.autoConfigure()) {
                    requiresList << prerequisite.name();
                }
            }

            if (requiresList.count() > 0) {
                f << "Requires.private: " << requiresList.join(' ') << nl;
            }
        }

        f << "Cflags: -I${includedir}";
        if (hasLibInclude)
            f << " -I${libincludedir}";
        f << nl;
        return f;
    }

    void generatePkgConfig(const BuildPlan &plan) const override
    {
        File::save(pkgConfigName(plan), pkgConfig(plan));
    }

    bool refreshLinkerCache(const BuildPlan &plan) const override
    {
        if (cygwin_) return true;
        if (plan.installRoot() != "/") return true;
        String libInstallPath = plan.installPath("lib");
        if (isMultiArch_) libInstallPath = libInstallPath / machine_;
        if (!libInstallPath.isAbsolutePath()) libInstallPath = libInstallPath.absolutePathRelativeTo(Process::cwd());
        Format attr;
        attr << "ldconfig";
        attr << libInstallPath;
        String command = attr.join<String>(' ');
        return plan.shell().run(command);
    }

    bool strip(const BuildPlan &plan) const override
    {
        if (!(
            (plan.options() & BuildOption::Library) ||
            (plan.options() & BuildOption::Application) ||
            (plan.options() & BuildOption::Plugin)
        )) {
            return true;
        }
        Format attr;
        attr << "strip";
        attr << linkName(plan);
        String command = attr.join<String>(' ');
        return plan.shell().run(command);
    }

    void appendCompileOptions(Format &args, const BuildPlan &plan) const
    {
        if (plan.options() & BuildOption::Debug) args << "-g";
        if (plan.options() & BuildOption::Release) {
            args << "-DNDEBUG";
        }
        if (plan.optimize() != "") {
            if (plan.optimize() == "4")
                args << "-O3" << "-flto";
            else
                args << "-O" + plan.optimize();
        }
        if (plan.options() & BuildOption::Release) {
            if (plan.optimize() != "0") {
                args << "-mtune=generic" << "-fno-plt";
            }
        }

        if (plan.linkStatic()) args << "-static";
        else appendRelocationMode(args, plan);

        args
            << "-Wall"
            << "-pthread"
            << "-pipe"
            << "-D_FILE_OFFSET_BITS=64"
            << "-fvisibility-inlines-hidden"
            << "-ffile-prefix-map=" + plan.sourcePrefix() + "=src";

        if (cFlags_ != "" && args.at(0) == ccPath_) args << cFlags_;
        if (cxxFlags_ != "" && args.at(0) == cxxPath_) args << cxxFlags_;
        if (plan.bundle().count() > 0) {
            args << "-DCCBUILD_BUNDLE_PREFIX=" + bundlePrefix(plan);
        }
        if (plan.name() != "") {
            args << "-DCCBUILD_BUNDLE_VERSION=" + plan.version().toString();
        }
        for (const String &flags: plan.customCompileFlags()) {
            if (flags.contains("c++") && args.at(0) != cxxPath_) continue;
                // FIXME: workaround hack to prevent passing "-std=c++11" to the C compiler
            args << flags;
        }
        for (const String &path: plan.includePaths()) {
            args << "-I" + path;
        }
    }

    void appendLinkOptions(Format &args, const BuildPlan &plan) const
    {
        for (const String &flag: plan.customLinkFlags())
            args << flag;

        appendRelocationMode(args, plan);
        if (!(plan.options() & BuildOption::Library)) args << "-pie";

        //if (plan.options() & BuildOption::Plugin)
        //    args << "-Wl,--no-as-needed";

        if (plan.optimize() != "") {
            if (plan.optimize() == "4")
                args << "-O3" << "-flto";
            else
                args << "-O" + plan.optimize();
        }

        if (lFlags_ != "") args << lFlags_;

        List<String> libraryPaths = plan.libraryPaths();
        List<String> libraries = plan.libraries();

        if (!libraryPaths.contains("."))
            args << "-L.";

        for (const String &path: libraryPaths)
            args << "-L" + path;

        for (const String &name: libraries)
            args << "-l" + name;

        if (plan.containsCPlusPlus() && !cxxPath_.contains("++")) args << "-lstdc++";

        List<String> rpaths;
        if (rpathOverride_ != "")
            rpaths << "-rpath=" + rpathOverride_;
        else
            rpaths << "-rpath=$ORIGIN" << "-rpath=$ORIGIN/../lib";

        if (libraryPaths.count() > 0) {
            for (const String &path: libraryPaths)
                rpaths << "-rpath=" + path.absolutePathRelativeTo(Process::cwd());
        }

        // rpaths << "-rpath-link=" + Process::cwd();
        rpaths << "-rpath-link=$ORIGIN";

        if (cygwin_)
            args << "-Wl," + rpaths.join(',');
        else
            args << "-Wl,--enable-new-dtags," + rpaths.join(',');
    }

    static void appendRelocationMode(Format &args, const BuildPlan &plan)
    {
        if (!(
            plan.customCompileFlags().contains("-fPIC") ||
            plan.customCompileFlags().contains("-fPIE") ||
            plan.customCompileFlags().contains("-fpie") ||
            plan.customCompileFlags().contains("-fpic")
        )) {
            /*if (plan.options() & BuildOption::Library) args << "-fPIC";
            else args << "-fPIE";*/
            args << "-fPIC";
        }
    }

    String ccPath_;
    String cxxPath_;
    String machine_;
    String systemRoot_;
    Pattern dependencySplitPattern_ { "{1..:[\\:\\\\\n\r ]}" };
    String rpathOverride_;
    bool isMultiArch_ { true };
    String cFlags_;
    String cxxFlags_;
    String lFlags_;
    bool cygwin_ { false };
};

GnuToolChain::GnuToolChain(const String &compiler, const String &libInstallPath):
    ToolChain{new State{compiler, libInstallPath}}
{}

} // namespace cc::build
