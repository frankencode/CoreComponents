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
#include <cc/DirWalk>
#include <cc/Glob>
#include <cc/File>
#include <cc/Dir>
#include <cc/stdio>
#include <cc/DEBUG>

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

    {
        String sourcePrefix = BuildMap{}.commonPrefix();
        if (sourcePrefix == "") sourcePrefix = plan().projectPath();
        else sourcePrefix = sourcePrefix.canonicalPath();
        plan().sourcePrefix() = sourcePrefix;
    }

    if (!(plan().options() & (BuildOption::Simulate | BuildOption::Blindfold | BuildOption::Bootstrap))) {
        if (!globInterfaces()) {
            return success_ = false;
        }
    }
    globSources();

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.globbingStage().run()) {
            return success_ = false;
        }
    }

    return success_ = true;
}

bool GlobbingStage::globInterfaces()
{
    if (!(
        (plan().options() & BuildOption::Application) ||
        (plan().options() & BuildOption::Library) ||
        (plan().options() & BuildOption::Plugin)
    )) {
        return true;
    }

    const String interfacePath = plan().projectPath() / "interface";

    CC_INSPECT(interfacePath);

    for (const String &path: DirWalk{interfacePath, DirWalk::FilesOnly})
    {
        CC_INSPECT(path);
        String suffix = path.fileSuffix();
        String moduleName = path.copy(interfacePath.count() + 1, path.count() - suffix.count() - (suffix.count() > 0));
        moduleName.replace('/', '.');
        moduleName.replace("__", ".");
        CC_INSPECT(moduleName);
        if (!plan().interfaces().insert(moduleName, path)) {
            ferr() <<
                "Ambiguous module definition for \"" << moduleName << "\", bailing out, see:\n"
                "  " << plan().interfaces()(moduleName) << "\n"
                "  " << path << "\n";
            return false;
        }

        plan().sources().append(path);
    }

    return true;
}

void GlobbingStage::globSources()
{
    if (
        recipe().contains("source") &&
        (
            (plan().options() & BuildOption::Application) ||
            (plan().options() & BuildOption::Library) ||
            (plan().options() & BuildOption::Plugin) ||
            (plan().options() & BuildOption::Tools)
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

        if ((plan().options() & BuildOption::Lump) && sources.count() > 1)
        {
            String lumpPath = plan().projectPath() / (containsCPlusPlus ? ".lump.cc" : ".lump.c");
            {
                String command = Format{} << "cat " << sources.join(' ') << " > " << lumpPath;
                fout() << shell().beautify(command) << nl;
            }
            {
                File lump{lumpPath, FileOpen::Overwrite};
                for (const String &path: sources) {
                    File{path}.transferTo(lump);
                }
            }
            sources.deplete();
            sources.append(lumpPath);
        }

        plan().containsCPlusPlus() = containsCPlusPlus;

        if (plan().sources().count() == 0)
            plan().sources() = sources;
        else
            plan().sources().appendList(sources);
    }
}

List<String> GlobbingStage::globSources(const List<String> &patternList) const
{
    List<String> sources;

    for (const String &pattern: patternList) {
        for (const String &globbedPath: Glob{plan().sourcePath(pattern)}) {
            for (const String &sourcePath: DirWalk{globbedPath, DirWalk::FilesOnly}) {
                appendPath(sourcePath, &sources);
            }
        }
    }

    if (!(plan().options() & BuildOption::Bootstrap)) {
        for (const String &pattern: patternList) {
            for (const String &globbedPath: Glob{plan().prestagePath(pattern)}) {
                for (const String &sourcePath: DirWalk{globbedPath, DirWalk::FilesOnly}) {
                    appendPath(sourcePath, &sources);
                }
            }
        }
    }

    return sources;
}

void GlobbingStage::appendPath(const String &path, Out<List<String>> sources)
{
    if (!path.endsWith(".lump.cc") && !path.endsWith(".lump.c")) {
        sources->append(path);
    }
}

} // namespace cc::build
