/*
 * Copyright (C) 2021-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/GnuToolChain>
#include <cc/build/LinkJob>
// #include <cc/Pattern>
#include <cc/Format>
#include <cc/Process>
#include <cc/Command>
#include <cc/File>
#include <cc/Dir>
#include <cc/LineSource>
#include <cc/DEBUG>

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
        ldFlags_ = Process::env("LDFLAGS");
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
        String path = Process::env("SDKTARGETSYSROOT");
        if (path == "") path = Process::env("PKG_CONFIG_SYSROOT_DIR");

        if (path == "" && !compiler.contains("clang")) {
            path =
                Process{
                    Command{compiler + " -print-sysroot"}
                    .io(2, IoStream::error())
                }.output().readAll();
        }

        path.trim();

        return path;
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

    String objectFilePath(const BuildPlan &plan, const String &sourcePath) const override
    {
        String objectFilePath = sourcePath.longBaseName();
        if (plan.options() & BuildOption::Tools) {
            if (cygwin_) objectFilePath += ".exe";
        }
        else {
            objectFilePath += ".o";

            // disambiguate same source file names in different sub-directories belong to the same target
            {
                String path = sourcePath;
                if (path.startsWith(plan.projectPath())) {
                    path = path.copy(plan.projectPath().count() + 1, path.count());
                    if (path.contains('/')) {
                        path = path.cdUp();
                        path.replace('/', '_');
                        objectFilePath = path + '_' + objectFilePath;
                    }
                }
            }

            objectFilePath = plan.objectFilePath(objectFilePath);
        }

        return objectFilePath;
    }

    static String getDependenciesFilePath(const String &objectFilePath)
    {
        String path = objectFilePath.copy();
        String suffix = path.shortFileSuffix();
        if (suffix.count() > 0) {
            path.truncate(path.count() - suffix.count());
            path += 'd';
        }
        else {
            path += ".d";
        }
        return path;
    }

    #if 0
    bool readObjectDependencies(const BuildPlan &plan, const String &objectFilePath, Out<List<String>> dependencies) const override
    {
        const String dependenciesFilePath = getDependenciesFilePath(objectFilePath);
        bool ok = true;
        try {
            List<String> parts = dependencySplitPattern_.breakUp(File{dependenciesFilePath}.map());
            List<String> deps;
            for (const String &s: parts) { // FIXME: simplistic workaround for modules support
                if (
                    s.endsWith(".cc") ||
                    s.endsWith(".cxx") ||
                    s.endsWith(".cpp")
                ) {
                    deps.append(s);
                    continue;
                }
            }
            dependencies = deps;
            ok = (deps.count() > 0);
        }
        catch (SystemResourceError &ex)
        {
            ok = false;
        }

        CC_INSPECT(objectFilePath);
        CC_INSPECT(dependencies->join(", "));

        return ok;
    }
    #endif

    bool readObjectDependencies(const BuildPlan &plan, const String &objectFilePath, Out<List<String>> dependencies) const override
    {
        bool ok = true;
        try {
            const String dependenciesFilePath = getDependenciesFilePath(objectFilePath);
            // CC_INSPECT(dependenciesFilePath);
            String text = File{dependenciesFilePath}.map().replaced("\\\n", "");
            // CC_INSPECT(text);
            List<String> deps;
            for (const String &line: LineSource{text}) {
                // CC_INSPECT(line);
                List<String> parts = line.split(':');
                if (parts.count() != 2) continue;
                // CC_INSPECT(parts(0));
                if (parts(0).contains(objectFilePath)) {
                    // CC_INSPECT(parts(1));
                    String value = parts(1);
                    value.trim();
                    value.simplify();
                    List<String> list = value.split(' ');
                    for (const String &item: list) {
                        if (item.endsWith(".gcm")) continue;
                        if (item.contains("gcm.cache/")) continue;
                        if (!item.startsWith(plan.sourcePrefix())) continue;
                        // CC_INSPECT(item);
                        deps.append(item);
                    }
                }
            }

            dependencies = deps;
            ok = (deps.count() > 0);
        }
        catch (SystemResourceError &ex)
        {
            ok = false;
        }

        CC_INSPECT(objectFilePath);
        CC_INSPECT(ok);
        CC_INSPECT(dependencies->join(", "));

        return ok;
    }

    String headerUnitCompileCommand(const BuildPlan &plan, const String &source) const override
    {
        Format args;
        args << cxxPath_;
        args << "-fmodule-header";
        args << "-xc++-header";
        appendCompileOptions(args, plan);
        args << source;
        return args.join<String>(' ');
    }

    String interfaceUnitCompileCommand(const BuildPlan &plan, const String &source) const override
    {
        Format args;
        args << cxxPath_;
        args << "-c";
        args << "-fmodule-only";
        args << "-xc++";
        appendCompileOptions(args, plan);
        args << source;
        return args.join<String>(' ');
    }

    String compileCommand(const BuildPlan &plan, const String &sourcePath, const String &objectFilePath) const override
    {
        Format args;
        args << compiler(sourcePath);
        args << "-c" << "-o" << objectFilePath;
        args << "-MMD";
        appendCompileOptions(args, plan);
        args << sourcePath;
        return args.join<String>(' ');
    }

    Job createCompileJob(const BuildPlan &plan, const ObjectFile &objectFile) const override
    {
        return Job{compileCommand(plan, objectFile.sourcePath(), objectFile.objectFilePath())};
    }

    Job createCompileLinkJob(const BuildPlan &plan, const ObjectFile &objectFile) const override
    {
        Format args;
        args << compiler(objectFile.sourcePath());
        args << "-o" << linkName(objectFile);
        args << "-MMD";
        appendCompileOptions(args, plan);
        if (plan.linkStatic()) args << "-static";
        args << "-pthread";
        args << objectFile.sourcePath();
        appendLinkOptions(args, plan);
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

    String linkName(const ObjectFile &objectFile) const override
    {
        String name = objectFile.sourcePath().baseName();

        if (cygwin_) {
            name = name + ".exe";
        }

        return name;
    }

    String linkCommand(const BuildPlan &plan) const override
    {
        String name = plan.name();
        Version version = plan.version();
        List<ObjectFile> objectFiles = plan.objectFiles();

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

        List<String> objectFilePaths;
        for (const ObjectFile &objectFile: objectFiles) {
            objectFilePaths << objectFile.objectFilePath();
        }
        args.appendList(objectFilePaths.sorted());

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
        // args << "-MMD";
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
        return plan.name() + ".pc";
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
        f << "Libs: -L${libdir} -l" << plan.name();
        for (const String &flag: plan.customLinkFlags()) {
            f << " " << flag;
        }
        f << nl;

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
            }

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
        if (hasLibInclude) f << " -I${libincludedir}";
        for (const String &flag: plan.customCompileFlags()) {
            f << " " << flag;
        }
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
        if (plan.options() & BuildOption::Release) args << "-DNDEBUG";
        if (plan.optimize() != "") args << "-O" + plan.optimize();
        if (plan.options() & BuildOption::Release) {
            if (plan.optimize() != "0") {
                args << "-flto" << "-mtune=generic" << "-fno-plt";
            }
        }

        if (plan.linkStatic()) args << "-static";
        else appendRelocationMode(args, plan);

        args
            << "-Wall"
            << "-pthread"
            << "-pipe"
            << "-D_FILE_OFFSET_BITS=64"
            << "-fvisibility-inlines-hidden";

        if (plan.options() & BuildOption::Release) {
            args << "-ffile-prefix-map=" + plan.sourcePrefix() + "=src";
        }

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

    /** \todo Make '-flto' work properly without overcommiting beyond BuildPlan::concurrency().
      */
    void appendLinkOptions(Format &args, const BuildPlan &plan) const
    {
        for (const String &flag: plan.customLinkFlags())
            args << flag;

        appendRelocationMode(args, plan);
        if (!(plan.options() & BuildOption::Library)) args << "-pie";

        if (plan.optimize() != "") {
            args << "-flto=" + str(plan.concurrency()) << "-O" + plan.optimize();
        }

        if (ldFlags_ != "") {
            args << ldFlags_;
            if (ldFlags_.contains("--as-needed")) {
                args << "-Wl,--no-as-needed";
            }
        }

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
    // Pattern dependencySplitPattern_ { "{1..:[\\:\\\\\n\r \\|]}" };
    String rpathOverride_;
    bool isMultiArch_ { true };
    String cFlags_;
    String cxxFlags_;
    String ldFlags_;
    bool cygwin_ { false };
};

GnuToolChain::GnuToolChain(const String &compiler, const String &libInstallPath):
    ToolChain{new State{compiler, libInstallPath}}
{}

} // namespace cc::build
