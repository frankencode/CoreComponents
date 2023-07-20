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
#include <cc/FileInfo>
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
    FileInfo sourceStatus{probePath};

    if (!sourceStatus) {
        ferr() << plan().recipePath() << ": " << name << ":" << nl;
        ferr() << "  " << probe << ": no such file" << nl;
        return false;
    }

    if (!Dir::exists(plan().configPath())) {
        Dir::create(plan().configPath());
    }

    String baseName = probePath.baseName();
    String binPath = plan().configPath() / baseName;

    bool dirty = true;
    FileInfo binStatus = plan().shell().fileStatus(binPath);
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

    FileInfo sourceStatus = FileInfo{configurePath};
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
        FileInfo binStatus = plan().shell().fileStatus(binPath);
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
