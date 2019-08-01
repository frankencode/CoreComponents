/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/stdio>
#include <cc/Dir>
#include <cc/File>
#include <cc/FileStatus>
#include <cc/Process>
#include <cc/meta/yason>
#include "BuildPlan.h"
#include "ConfigureShell.h"
#include "ConfigureStage.h"

namespace ccbuild {

bool ConfigureStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    BuildStageGuard guard{this};

    for (BuildPlan *prerequisite: plan()->prerequisites()) {
        if (!prerequisite->configureStage()->run()) {
            success_ = false;
            if (!(plan()->options() & BuildPlan::Package))
                return false;
        }
        makeUseOf(prerequisite);
    }

    if (!plan()->systemPrerequisitesByName()) return success_;

    bool firstLine = true;

    for (int i = 0; i < plan()->systemPrerequisitesByName()->count(); ++i)
    {
        String name = plan()->systemPrerequisitesByName()->keyAt(i);
        SystemPrerequisiteList *prerequisiteList = plan()->systemPrerequisitesByName()->valueAt(i);

        for (SystemPrerequisite *prerequisite: prerequisiteList)
        {
            Version version;

            if (prerequisite->autoConfigure()) {
                try {
                    prerequisite->customCompileFlags()->appendList(
                        configureShell(String{"pkg-config --cflags "} + prerequisite->name())->simplify()->split(' ')
                    );
                    prerequisite->customLinkFlags()->appendList(
                        configureShell(String{"pkg-config --libs "} + prerequisite->name())->simplify()->split(' ')
                    );
                    version = configureShell(String{"pkg-config --modversion "} + prerequisite->name());
                }
                catch (String &error) {
                    if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                        ferr()
                            << plan()->recipePath() << ": " << name << ":" << nl
                            << "  " << error << nl
                            << nl;
                    }
                    if (prerequisite->optional())
                        continue;
                    else
                        return success_ = false;
                }
            }
            else if (prerequisite->configure() != "") {
                String configure = prerequisite->configure();
                String output;
                if (!runConfigure(name, configure, &output)) {
                    if (plan()->options() & (BuildPlan::Verbose | BuildPlan::Configure)) {
                        ferr() << output;
                        ferr() << plan()->recipePath() << ": " << name << ":" << nl;
                        ferr() << "  " << configure << " failed" << nl;
                    }
                    if (prerequisite->optional())
                        continue;
                    else
                        return success_ = false;
                }
                Ref<MetaObject> object = Variant::cast<MetaObject *>(yason::parse(output));
                if (object) {
                    {
                        Ref<StringList> flags = getFlags(object, "compile-flags");
                        if (flags) prerequisite->customCompileFlags()->appendList(flags);
                    }
                    {
                        Ref<StringList> flags = getFlags(object, "link-flags");
                        if (flags) prerequisite->customLinkFlags()->appendList(flags);
                    }
                    {
                        Variant value = object->value("version");
                        if (value->type() == VariantType::String) version = Version{String{value}};
                        else if (value->type() == VariantType::Version) version = Version{value};
                    }
                }
            }
            else {
                try {
                    prerequisite->customCompileFlags()->appendList(
                        configureShell(prerequisite->compileFlagsConfigure())->simplify()->split(' ')
                    );
                    prerequisite->customLinkFlags()->appendList(
                        configureShell(prerequisite->linkFlagsConfigure())->simplify()->split(' ')
                    );
                    if (prerequisite->versionConfigure() != "") // FIXME, why needed?
                        version = configureShell(prerequisite->versionConfigure());
                }
                catch (String &error) {
                    if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                        ferr()
                            << plan()->recipePath() << ": " << name << ":" << nl
                            << "  " << error << nl
                            << nl;
                    }
                    if (prerequisite->optional())
                        continue;
                    else
                        return success_ = false;
                }
            }

            try {
                Version versionMin = prerequisite->versionMin();
                Version versionMax = prerequisite->versionMax();
                if (versionMin->isValid()) {
                    if (version < versionMin)
                        throw String{Format{} << "At least version " << versionMin << " is required (version " << version << " detected)"};
                }
                if (versionMax->isValid()) {
                    if (versionMax < version)
                        throw String{Format{} << "At most version " << versionMax << " is supported (version " << version << " detected)"};
                }
            }
            catch (String &error) {
                if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                    ferr()
                        << plan()->recipePath() << ": " << name << ":" << nl
                        << "  " << error << nl
                        << nl;
                }
                if (prerequisite->optional())
                    continue;
                else
                    return success_ = false;
            }

            if (prerequisite->probe() != "") {
                if (!probeBuild(name, prerequisite->probe())) {
                    prerequisite->setCascade(false);
                    continue;
                }
            }

            if (plan()->options() & (BuildPlan::Configure|BuildPlan::Verbose)) {
                if (
                    prerequisite->customCompileFlags()->count() > 0 ||
                    prerequisite->customLinkFlags()->count() > 0 ||
                    version->isValid()
                ) {
                    if (firstLine) {
                        firstLine = false;
                        ferr() << plan()->recipePath() << ":" << nl;
                    }
                    String ns = prerequisite->origName();
                    if (ns == "" && prerequisite->configure() != "") ns = prerequisite->configure()->baseName();
                    if (ns != "") ns += ".";
                    if (prerequisite->customCompileFlags()->count() > 0)
                        ferr() << "  " << ns << "compile-flags: " << prerequisite->customCompileFlags()->join(" ") << nl;
                    if (prerequisite->customLinkFlags()->count() > 0)
                        ferr() << "  " << ns << "link-flags: " << prerequisite->customLinkFlags()->join(" ") << nl;
                    if (version->isValid())
                        ferr() << "  " << ns << "version: " << version << nl;
                    ferr() << nl;
                }
            }

            if (prerequisite->libraries()) plan()->libraries()->appendList(prerequisite->libraries());
            plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
            plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
        }
    }

    StringList::makeUnique(plan()->customCompileFlags());
    StringList::makeUnique(plan()->customLinkFlags());

    return success_;
}

String ConfigureStage::configureShell(const String &shellCommand)
{
    return ConfigureShell::instance()->run(shellCommand);
}

void ConfigureStage::makeUseOf(BuildPlan *other)
{
    if (other->options() & BuildPlan::Library) {
        String path = other->projectPath();
        String defaultIncludePath = path->extendPath("include");
        if (Dir::exists(defaultIncludePath)) {
            if (!plan()->includePaths()->contains(defaultIncludePath))
                plan()->includePaths()->append(defaultIncludePath);
        }
        else if (!plan()->includePaths()->contains(path)) {
            plan()->includePaths()->append(path);
        }
        plan()->libraries()->append(other->name());

        if (other->systemPrerequisitesByName()) {
            for (int i = 0; i < other->systemPrerequisitesByName()->count(); ++i)
            {
                SystemPrerequisiteList *prerequisiteList = other->systemPrerequisitesByName()->valueAt(i);

                for (SystemPrerequisite *prerequisite: prerequisiteList)
                {
                    if (prerequisite->cascade()) {
                        plan()->customCompileFlags()->appendList(prerequisite->customCompileFlags());
                        plan()->customLinkFlags()->appendList(prerequisite->customLinkFlags());
                    }
                }
            }
        }
    }
    else if (other->options() & BuildPlan::Package) {
        for (BuildPlan *prerequisite: other->prerequisites()) {
            if (!(prerequisite->options() & BuildPlan::Plugin))
                makeUseOf(prerequisite);
        }
    }
}

bool ConfigureStage::probeBuild(const String &name, const String &probe) const
{
    String probePath = plan()->projectPath()->extendPath(probe);
    Ref<FileStatus> sourceStatus = FileStatus::read(probePath);

    if (!sourceStatus->isValid()) {
        ferr() << plan()->recipePath() << ": " << name << ":" << nl;
        ferr() << "  " << probe << ": no such file" << nl;
        return false;
    }

    if (!Dir::exists(plan()->configPath()))
        Dir::create(plan()->configPath());

    String baseName = probePath->baseName();
    String binPath = plan()->configPath()->extendPath(baseName);

    bool dirty = true;
    Ref<FileStatus> binStatus = plan()->shell()->fileStatus(binPath);
    if (binStatus->isValid()) {
        if (binStatus->lastModified() > sourceStatus->lastModified())
            dirty = false;
    }

    if (dirty) {
        String command = toolChain()->configureCompileCommand(plan(), probePath, binPath);
        Ref<Process> sub = Process::open(command);
        String output = sub->output()->readAll();
        int exitCode = sub->wait();
        if (exitCode != 0) {
            if (plan()->options() & (BuildPlan::Verbose /*| BuildPlan::Configure*/)) {
                ferr()
                    << command << nl
                    << output << nl;
            }
            return false;
        }
    }

    return true;
}

bool ConfigureStage::runConfigure(String name, String configure, String *output) const
{
    String configurePath = plan()->projectPath()->extendPath(configure);

    Ref<FileStatus> sourceStatus = FileStatus::read(configurePath);
    if (!sourceStatus->isValid()) {
        ferr() << plan()->recipePath() << ": " << name << ":" << nl;
        ferr() << "  " << configure << ": no such file" << nl;
        return false;
    }

    String configureText;
    String binPath;

    if (+(sourceStatus->mode() & FileMode::AnyExec)) {
        binPath = configurePath;
    }
    else {
        if (!Dir::exists(plan()->configPath()))
            Dir::create(plan()->configPath());

        String baseName = configurePath->baseName();
        binPath = plan()->configPath()->extendPath(baseName);

        bool dirty = true;
        Ref<FileStatus> binStatus = plan()->shell()->fileStatus(binPath);
        if (binStatus->isValid()) {
            if (binStatus->lastModified() > sourceStatus->lastModified())
                dirty = false;
        }

        if (dirty) {
            String command = toolChain()->configureCompileCommand(plan(), configurePath, binPath);
            Ref<Process> sub = Process::open(command);
            String output = sub->output()->readAll();
            int exitCode = sub->wait();
            if (exitCode != 0) {
                if (plan()->options() & (BuildPlan::Verbose | BuildPlan::Configure)) {
                    ferr()
                        << command << nl
                        << output << nl;
                }
                return false;
            }
        }
    }

    Ref<Process> sub = Process::open(binPath);
    *output = sub->output()->readAll();
    return sub->wait() == 0;
}

Ref<StringList> ConfigureStage::getFlags(const MetaObject *object, String propertyName)
{
    Variant value;
    if (object->lookup(propertyName, &value)) {
        if (value->type() == VariantType::String)
            return String(value)->split(" ");
        else if (value->type() == VariantType::List && value->itemType() == VariantType::String)
            return Variant::cast<StringList *>(value);
    }
    return Ref<StringList>();
}

} // namespace ccbuild
