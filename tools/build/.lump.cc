/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildParameters>

namespace cc::build {

BuildParameters::State::State(const MetaObject &object, const String &projectPath, BuildOption options)
{
    read(object, projectPath, options);
}

void BuildParameters::State::read(const MetaObject &object, const String &projectPath, BuildOption options)
{
    compiler_ = object("compiler").to<String>();
    optimize_ = object("optimize").to<String>();
    linkStatic_ = object("static").to<bool>();

    includePaths_ = object("include-paths").to<List<String>>();
    libraryPaths_ = object("library-paths").to<List<String>>();
    libraries_ = object("link").to<List<String>>();

    for (String &path: includePaths_) {
        if (path.isRelativePath())
            path = path.absolutePathRelativeTo(projectPath);
    }

    customCompileFlags_ = object("compile-flags").to<List<String>>();
    customLinkFlags_ = object("link-flags").to<List<String>>();

    for (const MetaObject &child: object.children()) {
        if (child.className() == "Debug") {
            if (options & BuildOption::Debug) readSpecific(child, projectPath, options);
        }
        else if (child.className() == "Release") {
            if (options & BuildOption::Release) readSpecific(child, projectPath, options);
        }
        else {
            #ifdef __linux
            if (child.className() == "Linux") readSpecific(child, projectPath, options);
            #endif
            #ifdef __OpenBSD__
            if (child.className() == "OpenBSD") readSpecific(child, projectPath, options);
            #endif
            #ifdef __FreeBSD__
            if (child.className() == "FreeBSD") readSpecific(child, projectPath, options);
            #endif
            #ifdef __NetBSD__
            if (child.className() == "NetBSD") readSpecific(child, projectPath, options);
            #endif
            #ifdef __DragonFly__
            if (child.className() == "DragonFly") readSpecific(child, projectPath, options);
            #endif
            #ifdef __sun
            if (child.className() == "Solaris") readSpecific(child, projectPath, options);
            #endif
            #ifdef __MACH__
            if (child.className() == "Mach") readSpecific(child, projectPath, options);
            #endif
            #ifdef __hpux
            if (child.className() == "HPUX") readSpecific(child, projectPath, options);
            #endif
            #ifdef __CYGWIN32__
            if (child.className() == "Cygwin") readSpecific(child, projectPath, options);
            #endif
            #ifdef __MINGW32__
            if (child.className() == "MinGW") readSpecific(child, projectPath, options);
            #endif
        }
    }
}

void BuildParameters::State::readSpecific(const MetaObject &object, const String &projectPath, BuildOption options)
{
    readSpecific(BuildParameters{object, projectPath, options});
}

void BuildParameters::State::readSpecific(const BuildParameters &specific)
{
    if (specific.compiler() != "") compiler_ = specific.compiler();
    if (specific.optimize() != "") optimize_ = specific.optimize();
    if (specific.linkStatic()) linkStatic_ = true;

    includePaths_.appendList(specific.includePaths());
    libraryPaths_.appendList(specific.libraryPaths());
    libraries_.appendList(specific.libraries());

    customCompileFlags_.appendList(specific.customCompileFlags());
    customLinkFlags_.appendList(specific.customLinkFlags());
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobServer>

namespace cc::build {

struct JobServer::State: public Object::State
{
    State(const Channel<Job> &requestChannel, const Channel<Job> &replyChannel):
        requestChannel_{requestChannel},
        replyChannel_{replyChannel},
        thread_{[&]{ run(); }}
    {
        thread_.start();
    }

    ~State()
    {
        requestChannel_.shutdown();
        thread_.wait();
    }

    void run()
    {
        for (Job job; requestChannel_.popFront(&job);) {
            job.run();
            replyChannel_.pushBack(job);
        }
    }

    Channel<Job> requestChannel_;
    Channel<Job> replyChannel_;
    Thread thread_;
};

JobServer::JobServer(const Channel<Job> &requestChannel, const Channel<Job> &replyChannel):
    Object{new State{requestChannel, replyChannel}}
{}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/ConfigureStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/ConfigureShell>
#include <cc/build/BuildShell>
#include <cc/build/ToolChain>
#include <cc/FileStatus>
#include <cc/Dir>
#include <cc/Process>
#include <cc/Version>
#include <cc/yason>
#include <cc/stdio>

namespace cc::build {

bool ConfigureStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.configureStage().run()) {
            success_ = false;
            if (!(plan().options() & BuildOption::Package))
                return false;
        }
        makeUseOf(prerequisite);
    }

    if (plan().systemPrerequisitesByName().count() == 0) return success_;

    bool firstLine = true;

    try {
        for (KeyValue<String, SystemPrerequisite> &item: plan().systemPrerequisitesByName())
        {
            String name = item.key();
            SystemPrerequisite &prerequisite = item.value();
            Version version;

            if (prerequisite.autoConfigure()) {
                try {
                    prerequisite.customCompileFlags().appendList(
                        configureShell(String{"pkg-config --cflags "} + prerequisite.name()).simplified().split(' ')
                    );
                    prerequisite.customLinkFlags().appendList(
                        configureShell(String{"pkg-config --libs "} + prerequisite.name()).simplified().split(' ')
                    );
                    version = configureShell(String{"pkg-config --modversion "} + prerequisite.name());
                }
                catch (String &error) {
                    if (plan().options() & (BuildOption::Configure|BuildOption::Verbose)) {
                        ferr()
                            << plan().recipePath() << ": " << name << ":" << nl
                            << "  " << error << nl
                            << nl;
                    }
                    if (prerequisite.optional())
                        continue;
                    else
                        throw prerequisite;
                }
            }
            else if (prerequisite.configure() != "") {
                String configure = prerequisite.configure();
                String output;
                if (!runConfigure(name, configure, &output)) {
                    if (plan().options() & (BuildOption::Verbose|BuildOption::Configure)) {
                        ferr() << output;
                        ferr() << plan().recipePath() << ": " << name << ":" << nl;
                        ferr() << "  " << configure << " failed" << nl;
                    }
                    if (prerequisite.optional())
                        continue;
                    else
                        throw prerequisite;
                }
                Variant value = yasonParse(output);
                if (value.is<MetaObject>()) {
                    MetaObject object = value.to<MetaObject>();
                    {
                        List<String> flags = getFlags(object, "compile-flags");
                        prerequisite.customCompileFlags().appendList(flags);
                    }
                    {
                        List<String> flags = getFlags(object, "link-flags");
                        prerequisite.customLinkFlags().appendList(flags);
                    }
                    {
                        Variant value = object("version");
                        if (value.is<String>()) version = Version{value.to<String>()};
                        else if (value.is<Version>()) version = value.to<Version>();
                    }
                }
            }
            else {
                try {
                    prerequisite.customCompileFlags().appendList(
                        configureShell(prerequisite.compileFlagsConfigure()).simplified().split(' ')
                    );
                    prerequisite.customLinkFlags().appendList(
                        configureShell(prerequisite.linkFlagsConfigure()).simplified().split(' ')
                    );
                    if (prerequisite.versionConfigure() != "") // FIXME, why needed?
                        version = configureShell(prerequisite.versionConfigure());
                }
                catch (String &error) {
                    if (plan().options() & (BuildOption::Configure|BuildOption::Verbose)) {
                        ferr()
                            << plan().recipePath() << ": " << name << ":" << nl
                            << "  " << error << nl
                            << nl;
                    }
                    if (prerequisite.optional())
                        continue;
                    else
                        throw prerequisite;
                }
            }

            try {
                Version versionMin = prerequisite.versionMin();
                Version versionMax = prerequisite.versionMax();
                if (versionMin.isValid()) {
                    if (version < versionMin)
                        throw String{Format{} << "At least version " << versionMin << " is required (version " << version << " detected)"};
                }
                if (versionMax.isValid()) {
                    if (versionMax < version)
                        throw String{Format{} << "At most version " << versionMax << " is supported (version " << version << " detected)"};
                }
            }
            catch (String &error) {
                if (plan().options() & (BuildOption::Configure|BuildOption::Verbose)) {
                    ferr()
                        << plan().recipePath() << ": " << name << ":" << nl
                        << "  " << error << nl
                        << nl;
                }
                if (prerequisite.optional())
                    continue;
                else
                    throw prerequisite;
            }

            if (prerequisite.probe() != "") {
                if (!probeBuild(name, prerequisite.probe())) {
                    prerequisite.setCascade(false);
                    continue;
                }
            }

            if (plan().options() & (BuildOption::Configure|BuildOption::Verbose)) {
                if (
                    prerequisite.customCompileFlags().count() > 0 ||
                    prerequisite.customLinkFlags().count() > 0 ||
                    version.isValid()
                ) {
                    if (firstLine) {
                        firstLine = false;
                        ferr() << plan().recipePath() << ":" << nl;
                    }
                    String ns = prerequisite.origName();
                    if (ns == "" && prerequisite.configure() != "") ns = prerequisite.configure().baseName();
                    if (ns != "") ns += ".";
                    if (prerequisite.customCompileFlags().count() > 0)
                        ferr() << "  " << ns << "compile-flags: " << prerequisite.customCompileFlags().join(' ') << nl;
                    if (prerequisite.customLinkFlags().count() > 0)
                        ferr() << "  " << ns << "link-flags: " << prerequisite.customLinkFlags().join(' ') << nl;
                    if (version.isValid())
                        ferr() << "  " << ns << "version: " << version << nl;
                    ferr() << nl;
                }
            }

            plan().libraries().appendList(prerequisite.libraries());
            plan().customCompileFlags().appendList(prerequisite.customCompileFlags());
            plan().customLinkFlags().appendList(prerequisite.customLinkFlags());
        }
    }
    catch (SystemPrerequisite &)
    {
        success_ = false;
    }

    plan().customCompileFlags().sortUnique();
    plan().customLinkFlags().sortUnique();

    return success_;
}

String ConfigureStage::configureShell(const String &shellCommand)
{
    return ConfigureShell{}.run(shellCommand);
}

void ConfigureStage::makeUseOf(const BuildPlan &other)
{
    if (other.options() & BuildOption::Library) {
        String path = other.projectPath();
        String defaultIncludePath = path / "include";
        if (Dir::exists(defaultIncludePath)) {
            if (!plan().includePaths().contains(defaultIncludePath))
                plan().includePaths().append(defaultIncludePath);
        }
        else if (!plan().includePaths().contains(path)) {
            plan().includePaths().append(path);
        }
        plan().libraries().append(other.name());

        for (const auto &item: other.systemPrerequisitesByName()) {
            const SystemPrerequisite &prerequisite = item.value();
            if (prerequisite.cascade()) {
                plan().customCompileFlags().appendList(prerequisite.customCompileFlags());
                plan().customLinkFlags().appendList(prerequisite.customLinkFlags());
            }
        }
    }
    else if (other.options() & BuildOption::Package) {
        for (const BuildPlan &prerequisite: other.prerequisites()) {
            if (!(prerequisite.options() & BuildOption::Plugin) || (plan().options() & BuildOption::Plugin))
                makeUseOf(prerequisite);
        }
    }
}

bool ConfigureStage::probeBuild(const String &name, const String &probe) const
{
    String probePath = plan().projectPath() / probe;
    FileStatus sourceStatus{probePath};

    if (!sourceStatus) {
        ferr() << plan().recipePath() << ": " << name << ":" << nl;
        ferr() << "  " << probe << ": no such file" << nl;
        return false;
    }

    if (!Dir::exists(plan().configPath()))
        Dir::create(plan().configPath());

    String baseName = probePath.baseName();
    String binPath = plan().configPath() / baseName;

    bool dirty = true;
    FileStatus binStatus = plan().shell().fileStatus(binPath);
    if (binStatus.isValid()) {
        if (binStatus.lastModified() > sourceStatus.lastModified())
            dirty = false;
    }

    if (dirty) {
        String command = toolChain().configureCompileCommand(plan(), probePath, binPath);
        Process process{command};
        String output = process.output().readAll();
        int status = process.wait();
        if (status != 0) {
            if (plan().options() & BuildOption::Verbose) {
                ferr()
                    << command << nl
                    << output << nl;
            }
            return false;
        }
    }

    return true;
}

bool ConfigureStage::runConfigure(const String &name, const String &configure, Out<String> output) const
{
    String configurePath = plan().projectPath() / configure;

    FileStatus sourceStatus = FileStatus{configurePath};
    if (!sourceStatus.isValid()) {
        ferr() << plan().recipePath() << ": " << name << ":" << nl;
        ferr() << "  " << configure << ": no such file" << nl;
        return false;
    }

    String configureText;
    String binPath;

    if (sourceStatus.mode() & FileMode::AnyExec) {
        binPath = configurePath;
    }
    else {
        if (!Dir::exists(plan().configPath()))
            Dir::create(plan().configPath());

        String baseName = configurePath.baseName();
        binPath = plan().configPath() / baseName;

        bool dirty = true;
        FileStatus binStatus = plan().shell().fileStatus(binPath);
        if (binStatus.isValid()) {
            if (binStatus.lastModified() > sourceStatus.lastModified())
                dirty = false;
        }

        if (dirty) {
            String command = toolChain().configureCompileCommand(plan(), configurePath, binPath);
            Process process{command};
            String output = process.output().readAll();
            int status = process.wait();
            if (status != 0) {
                if (plan().options() & (BuildOption::Verbose | BuildOption::Configure)) {
                    ferr()
                        << command << nl
                        << output << nl;
                }
                return false;
            }
        }
    }

    Process process{binPath};
    output = process.output().readAll();
    return process.wait() == 0;
}

List<String> ConfigureStage::getFlags(const MetaObject &object, const String &propertyName)
{
    Variant value;
    if (object.members().lookup(propertyName, &value)) {
        if (value.is<String>())
            return value.to<String>().split(' ');
        else if (value.is<List<String>>())
            return value.to<List<String>>();
    }
    return List<String>{};
}

} // namespace cc::build
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/GlobbingStage>
#include <cc/build/ToolChain>
#include <cc/build/BuildMap>
#include <cc/DirWalker>
#include <cc/Glob>
#include <cc/File>
#include <cc/stdio>

namespace cc::build {

bool GlobbingStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;

    if (
        (plan().options() & BuildOption::Test) &&
        !(plan().options() & BuildOption::BuildTests)
    ) {
        return success_ = true;
    }

    if (
        recipe().contains("source") && // FIXME: that is always the case?
        (
            (plan().options() & BuildOption::Application) ||
            (plan().options() & BuildOption::Library) ||
            (plan().options() & BuildOption::Plugin)
        )
    ) {
        List<String> sources = globSources(plan().recipe("source").to<List<String>>());
        bool containsCPlusPlus = false;

        for (const String &source: sources) {
            String suffix = source.fileSuffix();
            if (suffix == "cc" || suffix == "cpp" || suffix == "cxx" || suffix == "mm") {
                containsCPlusPlus = true;
                break;
            }
        }

        if (
            (plan().options() & BuildOption::Lump) &&
            sources.count() > 1
        ) {
            String lumpPath = plan().projectPath() / (containsCPlusPlus ? ".lump.cc" : ".lump.c");
            {
                File lump{lumpPath, FileOpen::Overwrite};
                for (const String &source: sources) {
                    File{source}.transferTo(lump);
                }
            }
            if (plan().options() & BuildOption::Verbose) {
                ferr() << "cat \\\n  " << sources.join(" \\\n  ") << "\\\n  > " << lumpPath << nl;
            }
            sources = List<String>{lumpPath};
        }

        plan().containsCPlusPlus() = containsCPlusPlus;
        plan().sources() = sources;
    }

    {
        String sourcePrefix = BuildMap{}.commonPrefix();
        if (sourcePrefix == "") sourcePrefix = plan().projectPath();
        else sourcePrefix = sourcePrefix.canonicalPath();
        plan().sourcePrefix() = sourcePrefix;
    }

    if (recipe().contains("bundle")) {
        plan().bundle() = globSources(plan().recipe("bundle").to<List<String>>());
    }

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.globbingStage().run())
            return success_ = false;
    }

    return success_ = true;
}

void GlobbingStage::appendPath(const String &path, Out<List<String>> sources)
{
    if(
        !path.endsWith(".lump.cc") &&
        !path.endsWith(".lump.c")
    )
        sources->append(path);
}

List<String> GlobbingStage::globSources(const List<String> &patternList) const
{
    List<String> sources;
    for (const String &pattern: patternList) {
        Glob glob{plan().sourcePath(pattern)};
        for (String path; glob.read(&path);) {
            try {
                DirWalker walker{path};
                bool isDir = false;
                for (String path; walker.read(&path, &isDir);) {
                    if (!isDir) appendPath(path, &sources);
                }
            }
            catch (...) {
                appendPath(path, &sources);
            }
        }
    }
    if (!(plan().options() & BuildOption::Bootstrap)) {
        for (const String &pattern: patternList) {
            Glob glob{plan().prestagePath(pattern)};
            for (String path; glob.read(&path);) {
                try {
                    DirWalker walker{path};
                    bool isDir = false;
                    for (String path; walker.read(&path, &isDir);) {
                        if (!isDir) appendPath(path, &sources);
                    }
                }
                catch (...) {
                    if (path.startsWith("./")) { // \todo FIXME, why?!
                        path = path.copy(2, path.count());
                    }
                    appendPath(path, &sources);
                }
            }
        }
    }

    return sources;
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildStageGuard>
#include <cc/build/BuildStage>
#include <cc/Process>

namespace cc::build {

BuildStageGuard::BuildStageGuard(const BuildStage *stage):
    stage_{*stage}
{
    for (const String &command: stage_.preCommands()) {
        Process::exec(command);
    }
}

BuildStageGuard::~BuildStageGuard()
{
    if (stage_.success()) {
        for (const String &command: stage_.postCommands()) {
            Process::exec(command);
        }
    }
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CompileLinkStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/ToolChain>
#include <cc/build/InsightDatabase>
#include <cc/File>
#include <cc/stdio>

namespace cc::build {

bool CompileLinkStage::run()
{
    BuildStageGuard guard{this};

    JobScheduler scheduler = createScheduler();
    scheduleJobs(scheduler);

    for (Job job; scheduler.collect(&job);) {
        fout() << shell().beautify(job.command()) << nl;
        ferr() << job.outputText();
        if (job.status() == 0) {
            if (!job.finish()) {
                status_ = 1;
                return success_ = false;
            }
        }
        else {
            status_ = job.status();
            return success_ = false;
        }
    }

    return success_ = true;
}

void CompileLinkStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;
    if (plan().isSystemSource()) return;

    if (plan().options() & BuildOption::Test) {
        if (!(plan().options() & BuildOption::BuildTests))
            return;
    }

    if (outOfScope()) return;

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        prerequisite.compileLinkStage().scheduleJobs(scheduler);
    }

    if (plan().options() & BuildOption::Package) return;

    Job linkJob;
    if (!(plan().options() & BuildOption::Tools))
        linkJob = toolChain().createLinkJob(plan());

    InsightDatabase insightDatabase;
    if (plan().options() & BuildOption::Insight) {
        insightDatabase = InsightDatabase{plan().projectPath()};
    }

    bool dirty = false;

    for (const Module &module: plan().modules())
    {
        bool moduleDirty = module.dirty();

        if (plan().options() & BuildOption::Tools) {
            moduleDirty = moduleDirty || !shell().fileStatus(toolChain().linkName(module));
        }

        if (moduleDirty)
        {
            dirty = true;

            Job job;
            if (plan().options() & BuildOption::Tools) {
                job = toolChain().createCompileLinkJob(plan(), module);
                if (!(plan().options() & BuildOption::Simulate)) {
                    plan().registerLinkDerivative(job);
                }
            }
            else {
                job = toolChain().createCompileJob(plan(), module);
                job.registerDerivative(linkJob);
            }

            if (plan().options() & BuildOption::Simulate) {
                fout() << plan().shell().beautify(job.command()) << ((plan().concurrency() == 1) ? "\n" : " &\n");
            }
            else {
                scheduler.schedule(job);
            }

            if (insightDatabase) {
                insightDatabase.insert(module.sourcePath(), job.command(), module.modulePath());
            }
        }
        else if (insightDatabase) {
            insightDatabase.touch(module.modulePath());
        }
    }

    if (insightDatabase) {
        insightDatabase.sync();

        if (plan().options() & BuildOption::Verbose) {
            ferr() << "Updated insight database " << insightDatabase.path() << nl;
        }
    }

    if (
        plan().modules().count() > 0 &&
        (plan().options() & BuildOption::Simulate) &&
        plan().concurrency() != 1
    ) {
        fout() << "wait" << nl;
    }

    if (plan().options() & BuildOption::Tools) return;

    if (!dirty)
    {
        FileStatus productStatus = shell().fileStatus(toolChain().linkName(plan()));
        if (!productStatus) dirty = true;

        else {
            String previousLinkCommandPath = plan().previousLinkCommandPath();
            String previousLinkCommand = File::load(previousLinkCommandPath);
            String newLinkCommand = linkJob.command();

            if (newLinkCommand != previousLinkCommand) dirty = true;
            else {
                double productTime = productStatus.lastModified();
                for (const Module &module: plan().modules()) {
                    FileStatus moduleStatus = shell().fileStatus(module.modulePath());
                    if (moduleStatus.lastModified() > productTime) {
                        dirty = true;
                        break;
                    }
                }
                if (!dirty) {
                    FileStatus recipeStatus = shell().fileStatus(plan().recipePath());
                    if (recipeStatus) {
                        if (recipeStatus.lastModified() > productTime)
                            dirty = true;
                    }
                }
            }
        }
    }

    if (!dirty) return;

    if (plan().options() & BuildOption::Simulate) {
        fout() << plan().shell().beautify(linkJob.command()) << nl;
        plan().toolChain().createSymlinks(plan());
    }
    else {
        plan().registerLinkDerivative(linkJob);
        if (plan().options() & BuildOption::Library) {
            plan().setLibraryLinkJob(linkJob);
        }
        scheduler.schedule(linkJob);
    }
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildStage>
#include <cc/build/BuildPlan>
#include <cc/build/ConfigureShell>
#include <cc/build/ToolChain>
#include <cc/build/JobScheduler>

namespace cc::build {

const BuildShell &BuildStage::shell() const
{
    return plan().shell();
}

const MetaObject &BuildStage::recipe() const
{
    return plan().recipe();
}

const ToolChain &BuildStage::toolChain() const
{
    return plan().toolChain();
}

JobScheduler BuildStage::createScheduler() const
{
    return JobScheduler{plan().concurrency()};
}

bool BuildStage::outOfScope() const
{
    return
        (plan().options() & (BuildOption::Application|BuildOption::Tools)) &&
        (!plan().projectPath().startsWith(plan().scope()));
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/PreprocessStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/JobScheduler>
#include <cc/build/ToolChain>
#include <cc/stdio>

namespace cc::build {

bool PreprocessStage::run()
{
    BuildStageGuard guard{this};

    JobScheduler scheduler;
    scheduleJobs(scheduler);

    if (scheduler) {
        for (Job job; scheduler.collect(&job);) {
            if (job.status() == 0) {
                fout() << job.outputText();
            }
            else {
                ferr() << job.command() << nl;
                ferr() << job.outputText();
                status_ = job.status();
                return success_ = false;
            }
        }
    }

    return success_ = true;
}

void PreprocessStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;
    if (plan().isSystemSource()) return;

    if (outOfScope()) return;
    if (plan().options() & BuildOption::Test) return;

    if (!scheduler) scheduler = createScheduler();

    for (BuildPlan &prerequisite: plan().prerequisites())
        prerequisite.preprocessStage().scheduleJobs(scheduler);

    for (const Module &module: plan().modules()) {
        scheduler.schedule(
            toolChain().createPreprocessJob(plan(), module)
        );
    }
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CleanStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/ToolChain>

namespace cc::build {

bool CleanStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;
    if (plan().isSystemSource()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.cleanStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (!shell().clean(plan().modulePath())) return success_ = false;

    String product = toolChain().linkName(plan());

    if ((plan().options() & BuildOption::Application) || (plan().options() & BuildOption::Library))
        shell().unlink(product);

    if (plan().options() & BuildOption::Application)
        toolChain().cleanAliasSymlinks(plan(), product);

    if (plan().options() & BuildOption::Library) {
        if (!plan().linkStatic()) {
            toolChain().cleanLibrarySymlinks(plan(), product);
            if (plan().options() & BuildOption::Plugin)
                toolChain().cleanPluginSymlinks(plan(), toolChain().targetName(plan().extensionTarget()));
        }
    }

    if (plan().options() & BuildOption::Tools) {
        for (const Module &module: plan().modules())
            shell().unlink(toolChain().linkName(module));
    }

    return success_ = true;
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/LinkJob>
#include <cc/build/JobState>
#include <cc/File>

namespace cc::build {

struct LinkJob::State: public Job::State
{
    State(const BuildPlan &plan):
        Job::State{plan.toolChain().linkCommand(plan)},
        plan_{plan}
    {}

    bool run() override
    {
        if (!plan().toolChain().createSymlinks(plan())) return false;
        if (plan().options() & BuildOption::Library) plan().toolChain().generatePkgConfig(plan());
        if (!Job::State::run()) return false;
        File::save(plan().previousLinkCommandPath(), command_);
        return true;
    }

    const BuildPlan &plan() const { return plan_; }

    BuildPlan plan_;
};

LinkJob::LinkJob(const BuildPlan &plan):
    Job{new State{plan}}
{}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/RecipeProtocol>
#include <cc/MetaProtocolState>
#include <cc/Version>

namespace cc::build {

class BuildParametersPrototype: public MetaPrototype
{
public:
    BuildParametersPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        MetaPrototype{className, protocol}
    {
        insert("compiler", "");
        insert("optimize", "");
        insert("static", false);

        insert("include-paths", List<String>{});
        insert("library-paths", List<String>{});
        insert("link", List<String>{});

        insert("compile-flags", List<String>{});
        insert("link-flags", List<String>{});
    }
};

class SpecificBuildParametersPrototype: public BuildParametersPrototype
{
public:
    SpecificBuildParametersPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        BuildParametersPrototype{
            className,
            [=]{
                MetaProtocol target { protocol };

                const char *paramClasses[] = {
                    "Debug", "Release",
                    "Linux", "OpenBSD", "FreeBSD", "NetBSD", "DragonFly",
                    "MacOSX", "Solaris", "Mach", "HPUX", "Cygwin"
                };

                for (const char *className: paramClasses) {
                    target.define(BuildParametersPrototype{className});
                }

                return target;
            }()
        }
    {}
};

class UserStop: public MetaPrototype
{
public:
    UserStop(const String &className):
        MetaPrototype{className}
    {
        insert("execute", "");
    }
};

class BuildOptionsPrototype: public SpecificBuildParametersPrototype
{
protected:
    BuildOptionsPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        SpecificBuildParametersPrototype{
            className,
            [=]{
                MetaProtocol target = protocol;

                const char *stopClasses[] = {
                    "PreBuild",     "PostBuild",
                    "PreClean",     "PostClean",
                    "PreInstall",   "PostInstall",
                    "PreUninstall", "PostUninstall",
                };

                for (const char *className: stopClasses) {
                    target.define(UserStop{className});
                }

                return target;
            }()
        }
    {
        insert("use", List<String>{});
        insert("root", "/");
        insert("prefix", "");

        insert("debug", false);
        insert("release", false);

        insert("setup", false);
        insert("insight", false);
        insert("lump", false);

        insert("configure", false);
        insert("configure-list", false);
        insert("prepare", false);
        insert("glob", false);
        insert("preprocess", false);
        insert("clean", false);
        insert("install", false);
        insert("uninstall", false);
        insert("test", false);
        insert("test-run", false);
        insert("test-report", false);
        insert("test-args", "");

        insert("verbose", false);
        insert("jobs", -1);
        insert("test-run-jobs", -1);

        insert("simulate", false);
        insert("blindfold", false);
        insert("bootstrap", false);

        insert("query", "");
        insert("query-all", false);

        insert("pkg-config", false);
    }
};

class SystemPrerequisitePrototype: public SpecificBuildParametersPrototype
{
public:
    SystemPrerequisitePrototype():
        SpecificBuildParametersPrototype{"Dependency"}
    {
        insert("name", "");
        insert("optional", false);
        insert("cascade", false);

        insert("compile-flags-configure", "");
        insert("link-flags-configure", "");

        insert("version-configure", "");
        insert("version-min", Version{});
        insert("version-max", Version{});

        insert("probe", "");
        insert("configure", "");
    }
};

class PredicatePrototype;

class PredicateMetaProtocol: public MetaProtocol
{
public:
    PredicateMetaProtocol(const PredicatePrototype &prototype);

private:
    struct State;
};

class PredicatePrototype: public MetaPrototype
{
public:
    PredicatePrototype():
        MetaPrototype{"Predicate", PredicateMetaProtocol{*this}}
    {
        insert("source", List<String>{});
        insert("target", "");
        insert("create", "");
        insert("update", "");
        insert("remove", "");
        insert("clean", "");
    }
};

struct PredicateMetaProtocol::State: public MetaProtocol::State
{
    State(const PredicatePrototype &prototype):
        prototype_{prototype}
    {}

    bool lookup(const String &className, Out<MetaPrototype> prototype) const override
    {
        if (className == prototype_.className()) {
            prototype = prototype_;
            return true;
        }
        return false;
    }

    const PredicatePrototype &prototype_;
};

PredicateMetaProtocol::PredicateMetaProtocol(const PredicatePrototype &prototype):
    MetaProtocol{new State{prototype}}
{}

class ApplicationPrototype: public BuildOptionsPrototype
{
public:
    ApplicationPrototype(const String &className = "Application", const MetaProtocol &protocol = MetaProtocol{}):
        BuildOptionsPrototype{
            className,
            [=]{
                MetaProtocol target = protocol;
                target.define(SystemPrerequisitePrototype{});
                target.define(PredicatePrototype{});
                return target;
            }()
        }
    {
        insert("name", "");
        insert("description", "");
        insert("alias", List<String>{});
        insert("depends", List<String>{});
        insert("source", List<String>{"*.(cc|cpp|cxx|c)"});
        insert("bundle", List<String>{});
        insert("version", Version{});
    }
};

class TestPrototype: public ApplicationPrototype
{
public:
    TestPrototype(const String &className = "Test"):
        ApplicationPrototype{className}
    {
        remove("prefix");
        remove("install");
    }
};

class LibraryPrototype: public ApplicationPrototype
{
public:
    LibraryPrototype():
        ApplicationPrototype{"Library"}
    {}
};

class PluginPrototype: public ApplicationPrototype
{
public:
    PluginPrototype():
        ApplicationPrototype{"Plugin"}
    {
        insert("extend", "");
        insert("group", "");
    }
};

class ToolsPrototype: public ApplicationPrototype
{
public:
    ToolsPrototype():
        ApplicationPrototype{"Tools"}
    {
        remove("name");
    }
};

class TestsPrototype: public TestPrototype
{
public:
    TestsPrototype():
        TestPrototype{"Tests"}
    {
        remove("name");
    }
};

class PackagePrototype: public ApplicationPrototype
{
public:
    PackagePrototype():
        ApplicationPrototype{"Package"}
    {
        // insert("version", Version{});
        insert("include", List<String>{}); // FIXME: confusing
    }
};

struct RecipeProtocol::State: public MetaProtocol::State
{};

RecipeProtocol::RecipeProtocol()
{
    initOnce<State>();

    define(ApplicationPrototype{});
    define(TestPrototype{});
    define(LibraryPrototype{});
    define(PluginPrototype{});
    define(ToolsPrototype{});
    define(TestsPrototype{});
    define(PackagePrototype{});
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/InstallStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/CwdGuard>
#include <cc/build/ToolChain>
#include <cc/File>

namespace cc::build {

bool InstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;
    if (plan().isSystemSource()) return success_ = true;

    if (outOfScope()) return success_ = true;
    if (plan().options() & BuildOption::Test) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.installStage().run())
            return success_ = false;
        linkerCacheDirty_ = linkerCacheDirty_ || prerequisite.installStage().linkerCacheDirty_;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (plan().options() & BuildOption::Tools) {
        for (const Module &module: plan().modules()) {
            if (!installTool(module))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = installApplicationOrLibrary();
}

bool InstallStage::installTool(const Module &module)
{
    String product = toolChain().linkName(module);
    return shell().install(product, toolChain().installDirPath(plan()) / product);
}

bool InstallStage::installApplicationOrLibrary()
{
    BuildOption options = plan().options();
    String product = toolChain().linkName(plan());
    String installDirPath = toolChain().installDirPath(plan());
    String installFilePath = installDirPath / product;

    if (!shell().install(product, installFilePath)) return false;

    if (options & BuildOption::Application) {
        if (plan().alias().count() > 0) {
            CwdGuard guard{installDirPath, shell()};
            toolChain().createAliasSymlinks(plan(), product);
        }
    }
    else if (options & BuildOption::Library)
    {
        if (!plan().linkStatic()) {
            linkerCacheDirty_ = true;
            CwdGuard guard{installDirPath, shell()};
            toolChain().createLibrarySymlinks(plan(), product);
            if (options & BuildOption::Plugin)
                toolChain().createPluginSymlinks(plan(), toolChain().linkName(plan().extensionTarget()).baseName(), product);
        }

        if (!
            shell().installAll(
                plan().projectPath() / "include",
                toolChain().includePrefix(plan())
            )
        )
            return false;

        if (!
            shell().installAll(
                plan().projectPath() / "libinclude",
                toolChain().libIncludePrefix(plan())
            )
        )
            return false;

        String pcName = toolChain().pkgConfigName(plan());
        String pcInstallPath = toolChain().pkgConfigInstallDirPath(plan()) / pcName;
        if (File::exists(plan().userPkgConfigPath())) {
            if (!shell().install(plan().userPkgConfigPath(), pcInstallPath))
                return false;
        }
        else {
            if (!File::exists(pcName)) toolChain().generatePkgConfig(plan());
            if (!shell().install(pcName, pcInstallPath))
                return false;
        }
    }

    for (const String &bundlePath: plan().bundle()) {
        String relativePath = bundlePath.copy(plan().projectPath().count() + 1, bundlePath.count());
        shell().install(
            bundlePath,
            plan().installRoot() / toolChain().bundlePrefix(plan()) / relativePath
        );
    }

    return true;
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobScheduler>
#include <cc/build/JobServer>
#include <cc/build/Job>
#include <cc/System>
#include <cc/Channel>
#include <cc/AppendList>
#include <cc/Set>

namespace cc::build {

struct JobScheduler::State: public Object::State
{
    State(int concurrency):
        concurrency_{concurrency > 0 ? concurrency : System::concurrency()}
    {}

    void start()
    {
        if (started_) return;
        started_ = true;

        for (int i = 0; i < concurrency_; ++i) {
            serverPool_.pushBack(JobServer{requestChannel_, replyChannel_});
        }
    }

    void schedule(const Job &job)
    {
        if (job.countDown() == 0)
            requestChannel_.pushBack(job);
        else
            derivatives_.insert(job);

        ++totalCount_;
    }

    bool collect(Out<Job> finishedJob)
    {
        if (totalCount_ == 0) {
            finishedJob = Job{};
            return false;
        }

        start();

        if (finishCount_ == totalCount_) {
            finishedJob = Job{};
            return false;
        }

        Job job;
        if (!replyChannel_.popFront(&job)) return false;

        finishedJob = job;

        if (job.status() == 0) {
            for (Job derivative; job.getNextDerivative(&derivative);) {
                if (derivative.countDown() == 0) {
                    requestChannel_.pushBack(derivative);
                    derivatives_.remove(derivative);
                }
            }
        }
        else {
            status_ = job.status();
            serverPool_.deplete();
        }

        ++finishCount_;

        return true;
    }

    int concurrency_ { -1 };

    Channel<Job> requestChannel_;
    Channel<Job> replyChannel_;

    AppendList<JobServer> serverPool_;
    Set<Job> derivatives_;

    bool started_ { false };
    int status_ { 0 };
    int totalCount_ { 0 };
    int finishCount_ { 0 };
};

JobScheduler::JobScheduler(int concurrency):
    Object{new State{concurrency}}
{}

int JobScheduler::concurrency() const
{
    return me().concurrency_;
}

void JobScheduler::start()
{
    me().start();
}

void JobScheduler::schedule(const Job &job)
{
    me().schedule(job);
}

bool JobScheduler::collect(Out<Job> finishedJob)
{
    return me().collect(finishedJob);
}

int JobScheduler::status() const
{
    return me().status_;
}

int JobScheduler::totalCount() const
{
    return me().totalCount_;
}

int JobScheduler::finishCount() const
{
    return me().finishCount_;
}

const JobScheduler::State &JobScheduler::me() const
{
    return Object::me.as<State>();
}

JobScheduler::State &JobScheduler::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobState>

namespace cc::build {

Job::Job(const String &command):
    Object{new State{command}}
{}

Job::Job(State *newState):
    Object{newState}
{}

String Job::command() const
{
    return me().command_;
}

int Job::status() const
{
    return me().status_;
}

String Job::outputText() const
{
    return me().outputText_;
}

void Job::registerDerivative(Job &job)
{
    me().derivatives_.pushBack(job);
    ++job.me().countDown_;
}

bool Job::getNextDerivative(Out<Job> derivative)
{
    if (me().derivatives_.count() == 0) return false;
    me().derivatives_.popFront(&derivative);
    --(*derivative).me().countDown_;
    return true;
}

int Job::countDown() const
{
    return me().countDown_;
}

bool Job::run()
{
    return me().run();
}

bool Job::finish()
{
    return me().finish();
}

const Job::State &Job::me() const
{
    return Object::me.as<State>();
}

Job::State &Job::me()
{
    return Object::me.as<State>();
}

} // namespaec cc::build
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
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/SystemPrerequisite>
#include <cc/str>

namespace cc::build {

SystemPrerequisite::State::State(const MetaObject &object, const String &projectPath, BuildOption options):
    BuildParameters::State{object, projectPath, options},
    name_{object("name").to<String>()},
    origName_{name_},
    optional_{object("optional").to<bool>()},
    cascade_{object("cascade").to<bool>()},
    compileFlagsConfigure_{object("compile-flags-configure").to<String>()},
    linkFlagsConfigure_{object("link-flags-configure").to<String>()},
    versionConfigure_{object("version-configure").to<String>()},
    versionMin_{object("version-min").to<Version>()},
    versionMax_{object("version-max").to<Version>()},
    probe_{object("probe").to<String>()},
    configure_{object("configure").to<String>()}
{
    if (name_ == "" && libraries_.count() == 1)
        name_ = libraries_.at(0);
    //if (name_ == "" && configure_ != "")
    //    name_ = configure_.baseName();
    if (name_ == "") name_ = hex(uint64_t(this));

    autoConfigure_ =
        origName_                   != "" &&
        compileFlagsConfigure_      == "" &&
        linkFlagsConfigure_         == "" &&
        versionConfigure_           == "" &&
        customCompileFlags_.count() == 0  &&
        customLinkFlags_.count()    == 0  &&
        probe_                      == "" &&
        configure_                  == "";
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildMap>
#include <cc/Map>

namespace cc::build {

struct BuildMap::State: public Object::State
{
    Map<String, BuildPlan> buildPlanByPath_;
    Map<String, String> libraryNameByPath_;
    Map<String, String> applicationNameByPath_;
};

BuildMap::BuildMap()
{
    initOnce<State>();
}

void BuildMap::insertPlan(const String &path, const BuildPlan &plan)
{
    me().buildPlanByPath_.insert(path, plan);
}

bool BuildMap::lookupPlan(const String &path, Out<BuildPlan> plan) const
{
    return me().buildPlanByPath_.lookup(path, &plan);
}

String BuildMap::commonPrefix() const
{
    if (me().buildPlanByPath_.count() == 0) return String{};

    return cc::commonPrefix(
        me().buildPlanByPath_.min().key(),
        me().buildPlanByPath_.max().key()
    );
}

bool BuildMap::registerLibrary(const String &name, const String &recipePath, Out<String> existingRecipePath)
{
    Locator pos;
    if (!me().libraryNameByPath_.insert(name, recipePath, &pos)) {
        existingRecipePath = me().libraryNameByPath_.at(pos).value();
        return false;
    }
    return true;
}

bool BuildMap::registerApplication(const String &name, const String &recipePath, Out<String> existingRecipePath)
{
    Locator pos;
    if (!me().applicationNameByPath_.insert(name, recipePath, &pos)) {
        existingRecipePath = me().applicationNameByPath_.at(pos).value();
        return false;
    }
    return true;
}

const BuildMap::State &BuildMap::me() const
{
    return Object::me.as<State>();
}

BuildMap::State &BuildMap::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/InsightDatabase>
#include <cc/File>
#include <cc/Format>
#include <cc/Variant>
#include <cc/MetaObject>
#include <cc/json>

namespace cc::build {

struct InsightDatabase::Entry
{
    String dirPath;
    String fileName;
    String command;
    bool inUse { false };
};

struct InsightDatabase::State final: public Object::State
{
    explicit State(const String &projectPath):
        path_{projectPath / "compile_commands.json"}
    {
        text_ = File::load(path_);
        Variant value;
        try { value = jsonParse(text_); }
        catch(...) {}
        if (value.is<List<Variant>>()) {
            List<Variant> list = value.to<List<Variant>>();
            for (const Variant &item: list) {
                Entry entry;
                String outputPath;
                if (!item.is<MetaObject>()) continue;
                {
                    MetaObject object = item.to<MetaObject>();
                    {
                        Variant value = object("command");
                        if (!value.is<String>()) continue;
                        entry.command = value.to<String>();
                    }
                    {
                        Variant value = object("directory");
                        if (!value.is<String>()) continue;
                        entry.dirPath = value.to<String>();
                    }
                    {
                        Variant value = object("file");
                        if (!value.is<String>()) continue;
                        entry.fileName = value.to<String>();
                    }
                    {
                        Variant value = object("output");
                        if (!value.is<String>()) continue;
                        outputPath = value.to<String>();
                    }
                }
                entries_(outputPath) = entry;
            }
        }
    }

    void insert(const String &sourcePath, const String &command, const String &outputPath)
    {
        entries_(outputPath) = Entry{sourcePath.cdUp(), sourcePath.fileName(), command, true};
        dirty_ = true;
    }

    void touch(const String &outputPath)
    {
        Locator pos;
        if (entries_.find(outputPath, &pos)) {
            entries_.at(pos).value().inUse = true;
        }
    }

    void sync()
    {
        if (!dirty_) return;
        dirty_ = false;

        Format f;
        f << "[\n";
        bool firstTime = true;
        for (const auto &pair: entries_) {
            const String &outputPath = pair.key();
            const Entry &entry = pair.value();
            if (!entry.inUse || outputPath == "") continue;
            if (firstTime) firstTime = false;
            else f << ",\n";
            f
                << "{ \"command\": \"" << entry.command << "\", "
                << "\"directory\": \"" << entry.dirPath << "\", "
                << "\"file\": \"" << entry.fileName << "\", "
                << "\"output\": \"" << outputPath << "\" }";
        }
        f << "\n]\n";

        String newText= f.join<String>();
        if (newText != text_) {
            File::save(path_, newText);
        }
    }

    String path_;
    String text_;
    Map<String, Entry> entries_;
    bool dirty_ { false };
};

InsightDatabase::InsightDatabase(const String &projectPath):
    Object{new State{projectPath}}
{}

void InsightDatabase::insert(const String &sourcePath, const String &command, const String &outputPath)
{
    me().insert(sourcePath, command, outputPath);
}

void InsightDatabase::touch(const String &outputPath)
{
    me().touch(outputPath);
}

void InsightDatabase::sync()
{
    me().sync();
}

String InsightDatabase::path() const
{
    return me().path_;
}

const InsightDatabase::State &InsightDatabase::me() const
{
    return Object::me.as<State>();
}

InsightDatabase::State &InsightDatabase::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/TestRunStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/ToolChain>
#include <cc/build/Module>
#include <cc/Process>
#include <cc/stdio>

namespace cc::build {

bool TestRunStage::run()
{
    bool report = plan().testReport();

    if (report) Process::setEnv("TEST_REPORT", "1");

    JobScheduler scheduler = JobScheduler{plan().testRunConcurrency()};
    scheduleJobs(scheduler);
    int testFailed = 0;

    for (Job job; scheduler.collect(&job);) {
        fout() << job.outputText();
        if (job.status() != 0) {
            ++testFailed;
            if (!report) {
                status_ = job.status();
                return success_ = false;
            }
        }
    }

    if (report) status_ = testFailed;
    return success_ = (testFailed == 0);
}

void TestRunStage::scheduleJobs(JobScheduler &scheduler)
{
    if (complete_) return;
    complete_ = true;

    if (!plan().goForBuild()) return;

    if (outOfScope()) return;

    for (BuildPlan &prerequisite: plan().prerequisites())
        prerequisite.testRunStage().scheduleJobs(scheduler);

    if (!(plan().options() & BuildOption::Test)) return;

    if (plan().options() & BuildOption::Tools) {
        for (Module &module: plan().modules()) {
            String command = "./" + toolChain().linkName(module);
            if (plan().testArgs() != "") command += " " + plan().testArgs();
            scheduler.schedule(Job{command});
        }
    }
    else {
        String command = toolChain().linkName(plan());
        if (plan().testArgs() != "") command += " " + plan().testArgs();
        scheduler.schedule(Job{command});
    }
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/PreparationStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/FileStatus>
#include <cc/Glob>
#include <cc/Pattern>
#include <cc/stdio>

namespace cc::build {

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (plan().options() & BuildOption::Test) {
        if (!(plan().options() & BuildOption::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.preparationStage().run())
            return success_ = false;
    }

    if (plan().predicates().count() == 0) return success_ = true;

    shell().mkdir(plan().prestagePath());

    for (const Predicate &predicate: plan().predicates())
    {
        JobScheduler scheduler;

        if (predicate.source().count() == 0) {
            String targetPath = plan().prestagePath(predicate.target().replaced("%", ""));
            if (plan().options() & BuildOption::Clean) {
                if (predicate.clean() != "") {
                    String command = expandCommand(predicate.clean(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
                continue;
            }
            if (predicate.create() != "") {
                if (!shell().fileStatus(targetPath)) {
                    String command = expandCommand(predicate.create(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        for (const String &sourceText: predicate.source())
        {
            String sourceExpression =
                plan().sourcePath(
                    sourceText.replaced("%", "(?@*)")
                );
            Pattern sourcePattern { sourceExpression };
            Glob glob{sourceExpression};
            for (const String &sourcePath: glob) {
                String name;
                if (sourceText.contains('%')) {
                    List<Range> captures;
                    if (sourcePattern.match(sourcePath, &captures)) {
                        if (captures.count() > 0) {
                            name = sourcePath.copy(captures.at(0));
                        }
                    }
                }
                else {
                    name = sourcePath.baseName();
                }
                String targetPath =
                    plan().prestagePath(
                        predicate.target().replaced("%", name)
                    );
                if (plan().options() & BuildOption::Clean) {
                    String command = expandCommand(predicate.clean(), sourcePath, targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                    continue;
                }
                if (
                    (plan().options() & BuildOption::Blindfold) ||
                    shell().fileStatus(targetPath).lastModified() < shell().fileStatus(sourcePath).lastModified()
                ) {
                    String command = expandCommand(predicate.update(), sourcePath, targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        if (predicate.remove() != "") {
            String targetExpression =
                plan().sourcePath(
                    predicate.target().replaced("%", "(?@*)")
                );
            Pattern targetPattern { targetExpression };
            Glob glob{targetExpression};
            for (const String &targetPath: glob) {
                String name;
                if (predicate.target().contains('%')) {
                    List<Range> captures;
                    if (targetPattern.match(targetPath, &captures)) {
                        if (captures.count() > 0) {
                            name = targetPath.copy(captures.at(0));
                        }
                    }
                }
                else {
                    name = targetPath.baseName();
                }
                bool sourceFound = false;
                for (const String &sourceText: predicate.source()) {
                    String sourcePath =
                        plan().sourcePath(
                            sourceText.replaced("%", name)
                        );
                    if (shell().fileStatus(sourcePath)) {
                        sourceFound = true;
                        break;
                    }
                }
                if (!sourceFound) {
                    String command = expandCommand(predicate.remove(), "", targetPath);
                    if (plan().options() & BuildOption::Simulate) {
                        fout() << shell().beautify(command) << nl;
                    }
                    else {
                        if (!scheduler) {
                            scheduler = createScheduler();
                            scheduler.start();
                        }
                        scheduler.schedule(Job{command});
                    }
                }
            }
        }

        if (scheduler) {
            for (Job job; scheduler.collect(&job);) {
                fout() << shell().beautify(job.command()) << nl;
                if (job.status() != 0) {
                    ferr() << job.outputText();
                    status_ = job.status();
                    return success_ = false;
                }
            }
        }
    }

    return true;
}

String PreparationStage::expandCommand(const String &command, const String &sourcePath, const String &targetPath) const
{
    String s = command;
    s.replace("$CFLAGS", plan().customCompileFlags().join(' '));
    s.replace("$@@", plan().prestagePath());
    s.replace("$<<", plan().projectPath());
    s.replace("$<", sourcePath);
    s.replace("$@", targetPath);
    return s;
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildPlan>
#include <cc/Bundle>
#include <cc/exceptions>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc::build;
    using namespace cc;

    String toolName = String{argv[0]}.fileName();
    int exitCode = 0;
    try {
        exitCode = BuildPlan{argc, argv}.run();
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]\n"
            "Build binaries from source DIR.\n"
            "\n"
            "Global Options:\n"
            "  -debug           compile-in debug information\n"
            "  -release         release mode (NDEBUG defined)\n"
            "  -static          build static libraries\n"
            "  -verbose         verbose output\n"
            "  -optimize        optimization level / strategy (0, 1, 2, 3, 4, s, g)\n"
            "  -root            file system root for installation (default: \"/\")\n"
            "  -prefix          installation prefix (default: '/usr/local')\n"
            "  -setup           setup convenience Makefile and scripts\n"
            "  -insight         generate compile_commands.json files (e.g. for clangd)\n"
            "  -lump            handle all sources of a target as a single source file\n"
            "\n"
            "Build Stage Options:\n"
            "  -configure       configure the dependencies and show results\n"
            "  -configure-list  list all system dependencies\n"
            "  -prepare         evaluate predicate rules\n"
            "  -glob            gather the list of source and package files\n"
            "  -preprocess      print preprocessed sources to stdout\n"
            "  -clean           remove all files generated by the build process\n"
            "  -install         install applications, libraries and bundled files\n"
            "  -uninstall       delete installed files\n"
            "  -test            build all tests\n"
            "  -test-run        run all tests until first fails\n"
            "  -test-run-jobs   number of concurrent jobs to spawn for running tests\n"
            "  -test-report     run all tests ($? = number of failed tests)\n"
            "  -test-args       list of arguments to pass to all tests\n"
            "\n"
            "Misc. Options:\n"
            "  -compiler        select compiler\n"
            "  -jobs            number of concurrent jobs to spawn\n"
            "  -simulate        print build commands without executing them\n"
            "  -blindfold       do not see any existing files\n"
            "  -bootstrap       write bootstrap script\n"
            "  -query           query given properties (e.g. 'name' or 'version')\n"
            "  -query-all       query all properties\n"
            "  -pkg-config      print package configuration (*.pc)\n"
            "  -version         print version\n"
        ) << toolName;
    }
    catch (VersionRequest &ex) {
        fout() << "v" << CC_BUNDLE_VERSION << nl;
    }
    catch (UsageError &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }
    #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }
    #endif
    return exitCode;
}
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
        if (options & BuildOption::Release) return "3";
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

    String installDirPath(const BuildPlan &plan) const override
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
        return plan.installPath(relativePath);
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
        f << "libdir=" << installDirPath(plan) << nl;
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

    void appendCompileOptions(Format &args, const BuildPlan &plan) const
    {
        if (plan.options() & BuildOption::Debug) args << "-g";
        if (plan.options() & BuildOption::Release) args << "-DNDEBUG";
        if (plan.optimize() != "") {
            if (plan.optimize() == "4")
                args << "-O3" << "-flto";
            else
                args << "-O" + plan.optimize();
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
            args << "-ffile-prefix-map=" + plan.sourcePrefix() + "=.";
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
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/DependencyCache>
#include <cc/build/Module>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/Format>
#include <cc/yason>

namespace cc::build {

struct DependencyCache::State: public Object::State
{
    State(const BuildPlan &plan):
        cachePath_{plan.modulePath("DependencyCache")}
    {
        File::establish(cachePath_);
        cacheTime_ = FileStatus{cachePath_}.lastModified();

        MetaObject dependencyCache;
        try {
            Variant value = yasonParse(File::load(cachePath_));
            if (value.is<MetaObject>()) dependencyCache = value.to<MetaObject>();
        }
        catch (TextError &)
        {}

        if (!dependencyCache) return;
        if (dependencyCache.className() != "DependencyCache") return;

        for (const auto &item: dependencyCache.members())
        {
            previousSources_.append(item.key());

            if (!plan.sources().contains(item.key())) continue;

            MetaObject object = item.value().to<MetaObject>();
            String command = object("analyseCommand").to<String>();
            String modulePath = object("modulePath").to<String>();
            List<String> dependencyPaths;
            {
                List<Variant> vl = object("dependencyPaths").to<List<Variant>>();
                for (const Variant &item: vl) dependencyPaths.append(item.to<String>());
                // Here List<Variant> is implicitly converted to List<String> -- this is expensive!
                //! \todo Have proper prototypes for the cache files.
            }

            String sourcePath = dependencyPaths.at(0);

            bool dirty = false;

            FileStatus objectStatus = plan.shell().fileStatus(modulePath);
            double objectTime = objectStatus.lastModified();

            for (const String &source: dependencyPaths)
            {
                FileStatus sourceStatus = plan.shell().fileStatus(source);
                if (!sourceStatus) {
                    dirty = true;
                    break;
                }
                double sourceTime = sourceStatus.lastModified();
                if (sourceTime > cacheTime_) {
                    dirty = true;
                    break;
                }
                if (objectStatus) {
                    if (sourceTime > objectTime) {
                        dirty = true;
                        break;
                    }
                }
            }

            if (dirty) continue;

            if (command != plan.toolChain().analyseCommand(plan, sourcePath)) continue;

            cache_.insert(
                item.key(),
                Module{command, modulePath, dependencyPaths, !objectStatus}
            );
        }
    }

    ~State()
    {
        String indent = String::allocate(4, ' ');

        Format text;
        text << "DependencyCache {\n";

        for (const auto &item: cache_)
        {
            const String &sourcePath = item.key();
            const Module &module = item.value();
            text
                << indent << "\"" << sourcePath << "\": Module {\n"
                << indent << indent << "analyseCommand: \"" << module.analyseCommand() << "\"\n"
                << indent << indent << "modulePath: \"" << module.modulePath() << "\"\n"
                << indent << indent << "dependencyPaths: [\n";

            const List<String> &dependencyPaths = module.dependencyPaths();
            for (const String &path: dependencyPaths) {
                text << indent << indent << indent << "\"" << path;
                if (!dependencyPaths.last(path))
                    text << "\",\n";
                else
                    text << "\"\n";
            }

            text
                << indent << indent << "]\n"
                << indent << "}\n";
        }
        text << "}\n";

        File::save(cachePath_, text);
    }

    String cachePath_;
    double cacheTime_ { -1 };
    Map<String, Module> cache_;
    List<String> previousSources_;
};

DependencyCache::DependencyCache(const BuildPlan &plan):
    Object{new State{plan}}
{}

double DependencyCache::cacheTime() const
{
    return me().cacheTime_;
}

List<String> DependencyCache::previousSources() const
{
    return me().previousSources_;
}

bool DependencyCache::lookup(const String &source, Out<Module> module)
{
    return me().cache_.lookup(source, &module);
}

void DependencyCache::insert(const String &source, const Module &module)
{
    me().cache_.insert(source, module);
}

const DependencyCache::State &DependencyCache::me() const
{
    return Object::me.as<State>();
}

DependencyCache::State &DependencyCache::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildPlan>
#include <cc/build/RecipeProtocol>
#include <cc/build/PreparationStage>
#include <cc/build/ConfigureStage>
#include <cc/build/GlobbingStage>
#include <cc/build/AnalyseStage>
#include <cc/build/PreprocessStage>
#include <cc/build/CompileLinkStage>
#include <cc/build/TestRunStage>
#include <cc/build/InstallStage>
#include <cc/build/UninstallStage>
#include <cc/build/CleanStage>
#include <cc/build/SystemPrerequisite>
#include <cc/build/Predicate>
#include <cc/build/Module>
#include <cc/build/GnuToolChain>
#include <cc/build/BuildShell>
#include <cc/build/ConfigureShell>
#include <cc/build/BuildMap>
#include <cc/build/Job>
#include <cc/MetaObject>
#include <cc/Version>
#include <cc/Map>
#include <cc/Set>
#include <cc/Arguments>
#include <cc/Process>
#include <cc/File>
#include <cc/Dir>
#include <cc/Crc32Sink>
#include <cc/JsonWriter>
#include <cc/ResourceGuard>
#include <cc/Format>
#include <cc/yason>
#include <cc/stdio>

namespace cc::build {

struct BuildPlan::State:
    public BuildParameters::State,
    public PreparationStage,
    public ConfigureStage,
    public GlobbingStage,
    public AnalyseStage,
    public PreprocessStage,
    public CompileLinkStage,
    public TestRunStage,
    public InstallStage,
    public UninstallStage,
    public CleanStage,
    public BuildShell
{
    State(int argc, char *argv[]):
        argv0_{argv[0]},
        projectPath_{"."}
    {
        Arguments arguments{argc, argv};
        List<String> items = arguments.items();

        if (items.count() > 0) {
            if (items.count() > 1) {
                throw UsageError{"Handling multiple source directories at once is not supported"};
            }
            projectPath_ = items.at(0);
        }

        projectPath_ = projectPath_.absolutePathRelativeTo(Process::cwd()).canonicalPath();
        recipePath_ = recipePath(projectPath_);

        recipe_ = yasonParse(File{recipePath_}.map(), RecipeProtocol{}).to<MetaObject>();
        arguments.validate(recipe_.members());
        arguments.override(&recipe_.members());

        readRecipe();

        Process::setEnv("PREFIX", installPrefix_);

        toolChain_ = GnuToolChain{compiler_, installPath("lib")};
        if (optimize_ == "") optimize_ = toolChain_.defaultOptimization(options_);

        scope_ = projectPath_;
    }

    State(const String &projectPath, const BuildPlan& parentPlan):
        toolChain_{parentPlan->toolChain_},
        projectPath_{projectPath},
        recipePath_{recipePath(projectPath)},
        scope_{parentPlan->scope_},
        concurrency_{parentPlan->concurrency_},
        disabled_{false}
    {
        ResourceGuard guard{recipePath_};
        recipe_ = yasonParse(File{recipePath_}.map(), RecipeProtocol{}).to<MetaObject>();
        readRecipe(parentPlan);
    }

    static String recipePath(const String &projectPath)
    {
        return projectPath / "Recipe";
    }

    String systemSourcePath() const
    {
        return toolChain_.systemRoot() / "usr/src";
    }

    void readRecipe(const BuildPlan &parentPlan = BuildPlan{})
    {
        if (recipe_.className() == "Application")  options_ |= BuildOption::Application;
        else if (recipe_.className() == "Library") options_ |= BuildOption::Library;
        else if (recipe_.className() == "Plugin")  options_ |= BuildOption::Library | BuildOption::Plugin;
        else if (recipe_.className() == "Test")    options_ |= BuildOption::Application | BuildOption::Test;
        else if (recipe_.className() == "Tools")   options_ |= BuildOption::Tools;
        else if (recipe_.className() == "Tests")   options_ |= BuildOption::Tools | BuildOption::Test;
        else if (recipe_.className() == "Package") options_ |= BuildOption::Package;

        name_ = recipe_("name").to<String>();
        if (name_ == "") name_ = projectPath_.baseName();
        alias_ = recipe_("alias").to<List<String>>();
        version_ = recipe_("version").to<Version>();
        installRoot_ = recipe_("root").to<String>();
        installPrefix_ = recipe_("prefix").to<String>();
        if (installPrefix_ == "") {
            installPrefix_ = (installRoot_ == "/") ? "/usr/local" : "/usr";
        }
        testArgs_ = recipe_("test-args").to<String>();

        checkDuplicateTargetNames();

        if (recipe_("debug"))       options_ |= BuildOption::Debug;
        if (recipe_("release"))     options_ |= BuildOption::Release;
        if (recipe_("simulate"))    options_ |= BuildOption::Simulate;
        if (recipe_("blindfold"))   options_ |= BuildOption::Blindfold;
        if (recipe_("bootstrap"))   options_ |= BuildOption::Bootstrap | BuildOption::Simulate | BuildOption::Blindfold;
        if (recipe_("test"))        options_ |= BuildOption::BuildTests;
        if (recipe_("test-run"))    options_ |= BuildOption::BuildTests;
        if (recipe_("test-report")) options_ |= BuildOption::BuildTests;
        if (recipe_("clean"))       options_ |= BuildOption::BuildTests;
        if (recipe_("verbose"))     options_ |= BuildOption::Verbose;
        if (recipe_("configure"))   options_ |= BuildOption::Configure;
        else if (recipe_("clean"))  options_ |= BuildOption::Clean;
        if (recipe_("insight"))     options_ |= BuildOption::Insight;
        if (recipe_("lump"))        options_ |= BuildOption::Lump;

        concurrency_ = recipe_("jobs").to<long>();
        testRunConcurrency_ = recipe_("test-run-jobs").to<long>();

        if (parentPlan) {
            options_ &= ~BuildOption::GlobalOptions;
            options_ |= parentPlan->options_ & BuildOption::GlobalOptions;
            concurrency_ = parentPlan->concurrency_;
            if (parentPlan->testRunConcurrency_ > 0 && testRunConcurrency_ < 0)
                testRunConcurrency_ = parentPlan->testRunConcurrency_;
            installRoot_ = parentPlan->installRoot_;
            installPrefix_ = parentPlan->installPrefix_;
            testArgs_ = parentPlan->testArgs_;
        }
        else {
            ConfigureShell{}.setVerbose(options_ & BuildOption::Verbose);
        }

        if (concurrency_ > 0 && testRunConcurrency_ < 0)
            testRunConcurrency_ = concurrency_;

        BuildParameters::State::read(recipe_, projectPath_, options_);

        String defaultIncludePath = projectPath_ / "include";
        if (Dir::exists(defaultIncludePath)) {
            if (!includePaths_.contains(defaultIncludePath))
                includePaths_.append(defaultIncludePath);
        }

        List<String> dependsList = recipe_("depends").to<List<String>>();
        if (dependsList.count() > 0) {
            for (const String &item: dependsList) {
                String name;
                Version versionMin, versionMax;
                if (item.contains(">=")) {
                    List<String> parts = item.split(">=");
                    name = parts.at(0).trimmed();
                    if (parts.count() > 1) versionMin = Version{parts.at(1).trimmed()};
                }
                else if (item.contains("<=")) {
                    List<String> parts = item.split("<=");
                    name = parts.at(0).trimmed();
                    if (parts.count() > 1) versionMax = Version{parts.at(1).trimmed()};
                }
                else {
                    name = item;
                }

                SystemPrerequisite prerequisite{name};
                prerequisite.setVersionMin(versionMin);
                prerequisite.setVersionMax(versionMax);
                systemPrerequisitesByName_.insert(name, prerequisite);
            }
        }

        if (recipe_.children().count() > 0) {
            for (const MetaObject &object: recipe_.children()) {
                if (object.className() == "Dependency") {
                    SystemPrerequisite p{object, projectPath_, options_};
                    if (systemPrerequisitesByName_.find(p.name())) {
                        throw UsageError{Format{"%%: Ambiguous system dependency '%%'"} << recipePath_ << p.name()};
                    }
                    systemPrerequisitesByName_.insert(p.name(), p);
                }
                else if (object.className() == "Predicate") {
                    readPredicate(object);
                }
                else if (object.className() == "PreBuild") {
                    CompileLinkStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostBuild") {
                    CompileLinkStage::postCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PreClean") {
                    CleanStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostClean") {
                    CleanStage::postCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PreInstall") {
                    InstallStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostInstall") {
                    InstallStage::postCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PreUninstall") {
                    UninstallStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostUninstall") {
                    UninstallStage::postCommands().append(object("execute").to<String>());
                }
            }
        }

        if (parentPlan) {
            optimize_ = parentPlan->optimize_;
            linkStatic_ = parentPlan->linkStatic_;
        }
    }

    void readPredicate(const MetaObject &object)
    {
        predicates_.append(Predicate{object});
        for (const MetaObject &child: object.children())
            readPredicate(child);
    }

    void checkDuplicateTargetNames()
    {
        if (name_ == "") return;

        String otherRecipePath;
        bool ok = true;

        if (options_ & BuildOption::Library)
            ok = BuildMap{}.registerLibrary(name_, recipePath_, &otherRecipePath);
        else if (options_ & BuildOption::Application)
            ok = BuildMap{}.registerApplication(name_, recipePath_, &otherRecipePath);

        if (!ok) {
            throw UsageError{
                Format{"Duplicate target name '%%' in\n  %%\n  and\n  %%"}
                << name_
                << otherRecipePath
                << recipePath_
            };
        }
    }

    String findPrerequisite(const String &prerequisitePath) const
    {
        if (prerequisitePath.isAbsolutePath()) {
            if (File::exists(recipePath(prerequisitePath))) return prerequisitePath;
            return String{};
        }

        {
            String candidatePath = systemSourcePath() / prerequisitePath;
            if (File::exists(recipePath(candidatePath))) return candidatePath;
        }

        for (String path = projectPath_; !path.isRootPath(); path = path.cdUp()) {
            String candidatePath = path / prerequisitePath;
            if (File::exists(recipePath(candidatePath))) {
                candidatePath = candidatePath.canonicalPath();
                if (candidatePath == projectPath_) continue;
                if (!(options_ & BuildOption::Package)) {
                    if (File::exists(candidatePath / "src" / "Recipe")) {
                        candidatePath = candidatePath / "src";
                    }
                }
                return candidatePath;
            }
        }

        return String{};
    }

    void readPrerequisites(BuildPlan &self)
    {
        if (prerequisitesRead_) return;
        prerequisitesRead_ = true;

        if ((options_ & BuildOption::Test) && !(options_ & BuildOption::BuildTests)) return;

        List<String> prerequisitePaths = recipe_("use").to<List<String>>();

        if (options_ & BuildOption::Package) {
            List<String> packageItems = recipe_("include").to<List<String>>();
            if (packageItems.count() > 0) prerequisitePaths = packageItems;
        }

        for (const String &prerequisitePath: prerequisitePaths) {
            String path = findPrerequisite(prerequisitePath);
            if (path == "") {
                throw UsageError{Format{} << recipePath_ << ": Failed to locate prerequisite '" << prerequisitePath << "'"};
            }
            BuildPlan plan = self.loadChild(path);
            plan.readPrerequisites();
            prerequisites_.append(plan);
        }

        if (options_ & BuildOption::Plugin) {
            String extensionTargetPath = recipe_("extend").to<String>();
            if (extensionTargetPath == "")
                throw UsageError{Format{} << recipePath_ << ": Please provide the path of a library to extend in Plugin.extend"};
            String path = findPrerequisite(extensionTargetPath);
            if (path == "")
                throw UsageError{Format{} << recipePath_ << ": Failed to locate library '" << extensionTargetPath << "'"};
            extensionTarget_ = self.loadChild(path);
            if (extensionTarget_.options() & BuildOption::Package) {
                extensionTarget_.readPrerequisites();
                for (const BuildPlan &prerequisite: extensionTarget_.prerequisites()) {
                    if (prerequisite.options() & BuildOption::Library) {
                        extensionTarget_ = prerequisite;
                        break;
                    }
                }
            }
            if (!(extensionTarget_.options() & BuildOption::Library))
                throw UsageError{Format{} << recipePath_ << ": '" << extensionTargetPath << "' (Plugin.extend) does not point to a library"};
            extensionTarget_.readPrerequisites();
            prerequisites_.appendList(extensionTarget_.prerequisites());
            prerequisites_.append(extensionTarget_);
        }

        findVersion();
    }

    void findVersion()
    {
        if (version_.isValid()) return;

        BuildPlan plan;
        String versionPath;

        String path = projectPath_.cdUp();

        for (; !path.isRootPath(); path = path.cdUp())
        {
            if (lookupCache(path, &plan)) {
                if (plan.version().isValid()) {
                    version_ = plan.version();
                    break;
                }
                continue;
            }

            try {
                File file{recipePath(path)};
                String text = file.readAll();
                Variant value = yasonParse(text);
                if (value.is<MetaObject>()) {
                    MetaObject recipe = value.to<MetaObject>();
                    Variant value;
                    if (recipe.lookup("version", &value)) {
                        version_ = Version{str(value)};
                        if (version_.isValid()) break;
                    }
                }
            }
            catch (...) {
            }
        }

        if (!version_.isValid()) {
            version_ = Version{0, 1, 0};
        }
    }

    void gatherAutoConfigureSystemPrerequisites(Set<String> &names)
    {
        if (configureListComplete_) return;
        configureListComplete_ = true;

        for (auto &item: systemPrerequisitesByName_) {
            SystemPrerequisite &p = item.value();
            if (p.autoConfigure() && p.origName() != "")
                names.insert(p.origName());
        }

        for (auto &plan: prerequisites_) {
            plan->gatherAutoConfigureSystemPrerequisites(names);
        }
    }

    String prestagePath(const String &precursor) const
    {
        return prestagePath_ / precursor;
    }

    String sourcePath(const String &source) const
    {
        if (projectPath_ == ".") return source;
        return projectPath_ / source;
    }

    void initModules()
    {
        if (modulesInitialized_) return;

        if ((options_ & BuildOption::Test) && !(options_ & BuildOption::BuildTests)) return;

        modulesInitialized_ = true;

        String suffix;
        {
            Format f;
            String absoulteProjectPath = projectPath_.absolutePathRelativeTo(Process::cwd());
            {
                Format h;
                String topLevel = sourcePrefix_.absolutePathRelativeTo(Process::cwd());
                for (
                    String path = absoulteProjectPath;
                    path != topLevel && path != "/" && path != toolChain_.systemRoot();
                    path = path.cdUp()
                ) {
                    h << path.fileName();
                }
                h << topLevel.fileName();
                f << hex(crc32(h.reversed().join('/')), 8);
            }
            if (options_ & BuildOption::Bootstrap)
                f << "$MACHINE";
            else
                f << toolChain_.machine();
            f << absoulteProjectPath.cdUp().fileName() + "_" + absoulteProjectPath.fileName();
            suffix = f.join<String>('-');
        }
        modulePath_ = ".modules-" + suffix;
        configPath_ = ".config-" + suffix;
        prestagePath_ = ".prestage-" + suffix;

        for (BuildPlan &prerequisite: prerequisites_)
            prerequisite->initModules();
    }

    String installPath(const String &relativeInstallPath) const
    {
        return installRoot_ / installPrefix_ / relativeInstallPath;
    }

    void setupBuildDir()
    {
        String options;
        {
            Format f;
            if (options_ & BuildOption::Debug) f << "-debug";
            if (options_ & BuildOption::Release) f << "-release";
            if (options_ & BuildOption::BuildTests) f << "-test";
            if (options_ & BuildOption::Verbose) f << "-verbose";
            if (options_ & BuildOption::Insight) f << "-insight";
            if (recipe_("optimize") != "") f << ("-optimize=" + optimize_);
            if (recipe_("prefix") != "") f << ("-prefix=" + installPrefix_);
            if (recipe_("root") != "/") f << ("-root=" + installRoot_);
            f << projectPath_;
            options = f.join<String>(' ');
        }

        Dir::establish(".setup");

        String toolPath = Process::execPath();
        if (toolPath == "") toolPath = "ccbuild";

        Format{
            File{".setup/configure", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -configure " << options << nl;

        Format{
            File{".setup/build", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " " << options << nl;

        Format{
            File{".setup/test_run", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -test-run " << options << nl;


        Format{
            File{".setup/clean", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -clean " << options << nl;

        Format{
            File{".setup/install", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -install " << options << nl;

        Format{
            File{".setup/uninstall", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -uninstall " << options << nl;

        try { File::unlink("build"); } catch (...) {};
        File::symlink(".setup/build", "build");

        File{"Makefile", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        .write(
            ".PHONY: build clean configure install uninstall\n"
            "\n"
            "build:\n"
            "\t./.setup/build\n"
            "\n"
            "test:\n"
            "\t./.setup/test_run\n"
            "\n"
            "clean:\n"
            "\t./.setup/clean\n"
            "\n"
            "configure:\n"
            "\t./.setup/configure\n"
            "\n"
            "install:\n"
            "\t./.setup/install\n"
            "\n"
            "uninstall:\n"
            "\t./.setup/uninstall\n"
            "\n"
        );
    }

    BuildPlan plan() const override { return Object::alias<BuildPlan>(this); }

    const BuildShell &shell() const { return *this; }

    const char *argv0_ { nullptr };
    ToolChain toolChain_;

    String projectPath_;
    String recipePath_;
    String scope_;
    String modulePath_;
    String configPath_;
    String prestagePath_;
    MetaObject recipe_;

    BuildOption options_  { BuildOption::None };
    int concurrency_ { -1 };
    int testRunConcurrency_ { -1 };
    bool disabled_ { false };

    String name_;
    List<String> alias_;
    Version version_;

    List<String> sources_;
    List<String> bundle_;
    List<Module> modules_;
    bool modulesInitialized_ { false };
    bool containsCPlusPlus_ { false };

    List<Predicate> predicates_;
    MultiMap<String, SystemPrerequisite> systemPrerequisitesByName_;
    List<BuildPlan> prerequisites_;
    bool prerequisitesRead_ { false };
    BuildPlan extensionTarget_;

    BuildParameters usage_;

    String sourcePrefix_;
    String installRoot_;
    String installPrefix_;
    String testArgs_;

    Job libraryLinkJob_;

    bool configureListComplete_ { false };
};

BuildPlan::BuildPlan(int argc, char *argv[]):
    BuildPlan{new State{argc, argv}}
{}

BuildPlan::BuildPlan(const String &projectPath, const BuildPlan &parentPlan):
    BuildPlan{new State{projectPath, parentPlan}}
{
    BuildMap{}.insertPlan(projectPath, *this);
}

BuildPlan::BuildPlan(State *newState):
    BuildParameters{newState}
{
    BuildMap{}.insertPlan(projectPath(), *this);
}

bool BuildPlan::lookupCache(const String &projectPath, Out<BuildPlan> plan)
{
    return BuildMap{}.lookupPlan(projectPath.absolutePathRelativeTo(Process::cwd()), &plan);
}

BuildPlan BuildPlan::loadChild(const String &projectPath)
{
    BuildPlan plan;
    if (lookupCache(projectPath, &plan)) return plan;
    return BuildPlan{projectPath, *this};
}

int BuildPlan::run()
{
    readPrerequisites();

    if (recipe("setup")) {
        me().setupBuildDir();
        return 0;
    }

    if (recipe("configure-list")) {
        Set<String> names;
        me().gatherAutoConfigureSystemPrerequisites(names);
        for (auto name: names)
            fout() << name << nl;
        return 0;
    }

    Process::setEnv("SOURCE", projectPath());

    me().initModules();

    configureStage().run();

    String query = recipe("query").to<String>();
    if (query != "") {
        queryVariables(query.split(','));
        return 0;
    }
    else if (recipe("query-all").to<bool>()) {
        queryVariables(queryableVariableNames());
        return 0;
    }

    if (options() & BuildOption::Configure) {
        return configureStage().success() ? 0 : 1;
    }

    if (options() & BuildOption::Bootstrap) {
        fout(
            "#!/bin/sh -ex\n"
            "SOURCE=$1\n"
            "MACHINE=$(%%)\n"
        ) << toolChain().machineCommand();
    }

    if (configureStage().success()) {
        if (!preparationStage().run()) {
            return 1;
        }
    }

    if (recipe("prepare").to<bool>()) return 0;

    String defaultIncludePath = projectPath() / "include";
    if (Dir::exists(defaultIncludePath)) {
        if (!me().includePaths_.contains(defaultIncludePath))
            me().includePaths_.append(defaultIncludePath);
    }

    if (!globbingStage().run()) return 1;

    if (recipe("glob").to<bool>()) return 0;

    if (recipe("pkg-config").to<bool>()) {
        if (options() & BuildOption::Package) {
            for (const BuildPlan &prerequisite: prerequisites()) {
                if (prerequisite.options() & BuildOption::Library) {
                    fout() << toolChain().pkgConfig(prerequisite);
                    break;
                }
            }
        }
        else if (options() & BuildOption::Library) {
            fout() << toolChain().pkgConfig(*this);
        }
        return 0;
    }

    if (!analyseStage().run()) {
        return 1;
    }

    if (recipe("preprocess").to<bool>()) {
        return preprocessStage().run();
    }

    if (options() & BuildOption::Clean) {
        return !cleanStage().run();
    }

    if (recipe("uninstall").to<bool>()) {
        return !uninstallStage().run();
    }

    if (!compileLinkStage().run()) {
        return 1;
    }

    bool testRun = recipe("test-run").to<bool>();
    bool testReport = recipe("test-report").to<bool>();
    if (testRun || testReport) {
        if (testRun) Process::setEnv("TEST_RUN", "1");
        if (testReport) Process::setEnv("TEST_REPORT", "1");
        if (!testRunStage().run()) {
            return testRunStage().status();
        }
    }

    if (recipe("install").to<bool>()) {
        if (!installStage().run()) return 1;
        if (installStage().linkerCacheDirty()) {
            if (!toolChain().refreshLinkerCache(*this))
                return 1;
        }
    }

    return 0;
}

void BuildPlan::readPrerequisites()
{
    return me().readPrerequisites(*this);
}

List<String> BuildPlan::queryableVariableNames()
{
    return List<String> {
        "name",
        "version",
        "source",
        "include-paths",
        "library-paths",
        "libraries",
        "custom-compile-flags",
        "custom-link-flags"
    };
}

void BuildPlan::queryVariables(const List<String> &names) const
{
    MetaObject variables;
    variables("name") = name();
    variables("version") = version();
    variables("source") = recipe("source");
    variables("include-paths") = includePaths();
    variables("library-paths") = libraryPaths();
    variables("libraries") = libraries();
    variables("custom-compile-flags") = customCompileFlags();
    variables("custom-link-flags") = customLinkFlags();
    JsonWriter{IoStream::output()}.writeObject(variables, names);
    IoStream::output().write("\n");
}

const ToolChain &BuildPlan::toolChain() const
{
    return me().toolChain_;
}

const BuildShell &BuildPlan::shell() const
{
    return me().shell();
}

const MetaObject &BuildPlan::recipe() const
{
    return me().recipe_;
}

Variant BuildPlan::recipe(const String &name) const
{
    return me().recipe_(name);
}

BuildOption BuildPlan::options() const
{
    return me().options_;
}

int BuildPlan::concurrency() const
{
    return me().concurrency_;
}

int BuildPlan::testRunConcurrency() const
{
    return me().testRunConcurrency_;
}

bool BuildPlan::goForBuild() const
{
    return (me().options_ & BuildOption::Package) || configureStage().success();
}

String BuildPlan::name() const
{
    return me().name_;
}

List<String> BuildPlan::alias() const
{
    return me().alias_;
}

Version BuildPlan::version() const
{
    return me().version_;
}

String BuildPlan::description() const
{
    return me().recipe_("description").to<String>();
}

bool BuildPlan::containsCPlusPlus() const
{
    return me().containsCPlusPlus_;
}

bool &BuildPlan::containsCPlusPlus()
{
    return me().containsCPlusPlus_;
}
const List<String> &BuildPlan::sources() const
{
    return me().sources_;
}

List<String> &BuildPlan::sources()
{
    return me().sources_;
}

const List<String> &BuildPlan::bundle() const
{
    return me().bundle_;
}

List<String> &BuildPlan::bundle()
{
    return me().bundle_;
}

const List<Module> &BuildPlan::modules() const
{
    return me().modules_;
}

List<Module> &BuildPlan::modules()
{
    return me().modules_;
}

String BuildPlan::projectPath() const
{
    return me().projectPath_;
}

String BuildPlan::systemSourcePath() const
{
    return me().systemSourcePath();
}

bool BuildPlan::isSystemSource() const
{
    return projectPath().startsWith(systemSourcePath());
}

String BuildPlan::sourcePath(const String &source) const
{
    return me().sourcePath(source);
}

String BuildPlan::recipePath() const
{
    return me().recipePath_;
}

String BuildPlan::userPkgConfigPath() const
{
    return projectPath() / toolChain().pkgConfigName(*this);
}

String BuildPlan::scope() const
{
    return me().scope_;
}

String BuildPlan::modulePath() const
{
    return me().modulePath_;
}

String BuildPlan::modulePath(const String &objectName) const
{
    return me().modulePath_ / objectName;
}

String BuildPlan::configPath() const
{
    return me().configPath_;
}

String BuildPlan::prestagePath() const
{
    return me().prestagePath_;
}

String BuildPlan::prestagePath(const String &precursor) const
{
    return me().prestagePath(precursor);
}

String BuildPlan::sourcePrefix() const
{
    return me().sourcePrefix_;
}

String &BuildPlan::sourcePrefix()
{
    return me().sourcePrefix_;
}

String BuildPlan::installRoot() const
{
    return me().installRoot_;
}

String BuildPlan::installPrefix() const
{
    return me().installPrefix_;
}

String BuildPlan::installPath(const String &relativeInstallPath) const
{
    return me().installPath(relativeInstallPath);
}

String BuildPlan::pluginPath(const String &targetLibName) const
{
    String path = targetLibName / "plugins";
    String group = me().recipe_("group");
    if (group != "") path = path / group;
    return path;
}

String BuildPlan::pluginReversePath() const
{
    String group = me().recipe_("group");
    return group != "" ? "../../.." : "../..";
}

String BuildPlan::previousLinkCommandPath() const
{
    return modulePath("LinkCommand");
}

void BuildPlan::setLibraryLinkJob(const Job &linkJob)
{
    me().libraryLinkJob_ = linkJob;
}

void BuildPlan::registerLinkDerivative(Job &linkJob)
{
    for (BuildPlan &prerequisite: prerequisites()) {
        if (prerequisite.options() & BuildOption::Package) {
            for (BuildPlan &child: prerequisite.prerequisites()) {
                if (child.options() & BuildOption::Library) {
                    if (child.libraryLinkJob()) {
                        child.libraryLinkJob().registerDerivative(linkJob);
                    }
                }
            }
        }
        else if (prerequisite.options() & BuildOption::Library) {
            if (prerequisite.libraryLinkJob()) {
                prerequisite.libraryLinkJob().registerDerivative(linkJob);
            }
        }
    }
}

String BuildPlan::testArgs() const
{
    return me().testArgs_;
}

bool BuildPlan::testReport() const
{
    return me().recipe_("test-report").to<bool>();
}

const List<Predicate> &BuildPlan::predicates() const
{
    return me().predicates_;
}

MultiMap<String, SystemPrerequisite> BuildPlan::systemPrerequisitesByName() const
{
    return me().systemPrerequisitesByName_;
}

const List<BuildPlan> &BuildPlan::prerequisites() const
{
    return me().prerequisites_;
}

List<BuildPlan> &BuildPlan::prerequisites()
{
    return me().prerequisites_;
}

const BuildPlan &BuildPlan::extensionTarget() const
{
    return me().extensionTarget_;
}

PreparationStage &BuildPlan::preparationStage()
{
    return me();
}

ConfigureStage &BuildPlan::configureStage()
{
    return me();
}

const ConfigureStage &BuildPlan::configureStage() const
{
    return me();
}

GlobbingStage &BuildPlan::globbingStage()
{
    return me();
}

AnalyseStage &BuildPlan::analyseStage()
{
    return me();
}

PreprocessStage &BuildPlan::preprocessStage()
{
    return me();
}

CompileLinkStage &BuildPlan::compileLinkStage()
{
    return me();
}

TestRunStage &BuildPlan::testRunStage()
{
    return me();
}

InstallStage &BuildPlan::installStage()
{
    return me();
}

UninstallStage &BuildPlan::uninstallStage()
{
    return me();
}

CleanStage &BuildPlan::cleanStage()
{
    return me();
}

Job BuildPlan::libraryLinkJob() const
{
    return me().libraryLinkJob_;
}

BuildPlan::State *BuildPlan::operator->()
{
    return &me();
}

const BuildPlan::State *BuildPlan::operator->() const
{
    return &me();
}

BuildPlan::State &BuildPlan::me()
{
    return Object::me.as<State>();
}

const BuildPlan::State &BuildPlan::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/ConfigureShell>
#include <cc/Process>
#include <cc/Command>
#include <cc/Format>

namespace cc::build {

struct ConfigureShell::State: public Object::State
{
    String run(const String &command)
    {
        if (command == "") return "";

        String output;
        if (cache_.lookup(command, &output))
            return output;

        Process process {
            Command{}
            .args({Process::env("SHELL"), "-c", command})
        };

        output = process.output().readAll();
        output.trim();

        int status = process.wait();
        if (status != 0) {
            throw String{
                Format{} << "Configure command failed with status = " << status << " (\"" << command << "\")"
            };
        }

        cache_.insert(command, output);

        return output;
    }

    bool verbose_ { false };
    Map<String> cache_;
};

ConfigureShell::ConfigureShell()
{
    initOnce<State>();
}

void ConfigureShell::setVerbose(bool on)
{
    me().verbose_ = on;
}

String ConfigureShell::run(const String &command)
{
    return me().run(command);
}

ConfigureShell::State &ConfigureShell::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/UninstallStage>
#include <cc/build/BuildStageGuard>
#include <cc/build/CwdGuard>
#include <cc/build/ToolChain>

namespace cc::build {

bool UninstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;

    if (plan().options() & BuildOption::Test) return success_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.uninstallStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    if (plan().options() & BuildOption::Tools) {
        for (const Module &module: plan().modules()) {
            if (!uninstallTool(module))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = uninstallApplicationOrLibrary();
}

bool UninstallStage::uninstallTool(const Module &module)
{
    String product = toolChain().linkName(module);
    return shell().unlink(toolChain().installDirPath(plan()) / product);
}

bool UninstallStage::uninstallApplicationOrLibrary()
{
    BuildOption options = plan().options();
    String product = toolChain().linkName(plan());
    String installDirPath = toolChain().installDirPath(plan());
    String installFilePath = installDirPath / product;

    if (!shell().unlink(installFilePath)) return false;

    if ((options & BuildOption::Library) && !plan().linkStatic()) {
        {
            CwdGuard guard{installDirPath, plan().shell()};
            toolChain().cleanLibrarySymlinks(plan(), product);
            if (plan().options() & BuildOption::Plugin)
                toolChain().cleanPluginSymlinks(plan(), toolChain().targetName(plan().extensionTarget()));
        }
        shell().unlinkAll(
            plan().projectPath() / "include",
            toolChain().includePrefix(plan())
        );
        shell().unlinkAll(
            plan().projectPath() / "libinclude",
            toolChain().libIncludePrefix(plan())
        );

        String pcName = toolChain().pkgConfigName(plan());
        String pcInstallPath = toolChain().pkgConfigInstallDirPath(plan()) / pcName;
        plan().shell().unlink(pcInstallPath);
    }

    if ((options & BuildOption::Application) && plan().alias().count() > 0) {
        CwdGuard guard{installDirPath, plan().shell()};
        toolChain().cleanAliasSymlinks(plan(), product);
    }

    return true;
}

} // namespace cc::build
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/AnalyseStage>
#include <cc/build/JobScheduler>
#include <cc/build/BuildStageGuard>
#include <cc/build/BuildShell>
#include <cc/build/DependencyCache>
#include <cc/stdio>

namespace cc::build {

bool AnalyseStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (!plan().goForBuild()) return success_ = true;

    if (
        (plan().options() & BuildOption::Test) &&
        !(plan().options() & BuildOption::BuildTests)
    ) {
        return success_ = true;
    }

    BuildStageGuard guard{this};

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.analyseStage().run())
            return success_ = false;
    }

    if (plan().options() & BuildOption::Package) return success_ = true;

    shell().mkdir(plan().modulePath());

    JobScheduler scheduler;

    DependencyCache dependencyCache { plan() };
    previousSources_ = dependencyCache.previousSources();
    cacheTime_ = dependencyCache.cacheTime();

    for (const String &source: plan().sources()) {
        Module module;
        if (dependencyCache.lookup(source, &module)) {
            plan().modules().append(module);
        }
        else {
            if (!scheduler) {
                scheduler = createScheduler();
                scheduler.start();
            }
            scheduler.schedule(toolChain().createAnalyseJob(plan(), source));
        }
    }

    if (!scheduler) return success_ = true;

    for (Job job; scheduler.collect(&job);) {
        if (plan().options() & BuildOption::Verbose || job.status() != 0)
            fout() << "# " << shell().beautify(job.command()) << nl;
        if (job.status() != 0) {
            ferr() << job.outputText();
            status_ = job.status();
            return success_ = false;
        }

        Module module = toolChain().finishAnalyseJob(plan(), job);
        dependencyCache.insert(module.sourcePath(), module);
        plan().modules().append(module);
    }

    plan().modules().sort(
        [](const Module &a, const Module &b) {
            return a.modulePath() < b.modulePath();
        }
    );

    return success_ = true;
}

} // namespace cc::build
