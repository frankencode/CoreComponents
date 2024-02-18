/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/GlobbingStage>
#include <cc/build/ImportManager>
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

    if (!gatherImports()) return success_ = false;
    gatherSources();

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.globbingStage().run()) {
            return success_ = false;
        }
    }

    return success_ = true;
}

bool GlobbingStage::gatherImports() const
{
    if (
        (plan().options() & BuildOption::Simulate) ||
        (plan().options() & BuildOption::Blindfold) ||
        (plan().options() & BuildOption::Bootstrap)
    ) {
        return true;
    }

    if (!(
        (plan().options() & BuildOption::Application) ||
        (plan().options() & BuildOption::Library) ||
        (plan().options() & BuildOption::Plugin)
    )) {
        return true;
    }

    const String importPrefix = plan().projectPath() / "import";

    CC_INSPECT(importPrefix);

    ImportManager manager;

    for (const String &source: DirWalk{importPrefix, DirWalk::FilesOnly})
    {
        String name = manager.moduleName(importPrefix, source);
        CC_INSPECT(source);
        CC_INSPECT(name);

        String otherSource;

        if (!manager.registerModule(name, source, &otherSource))
        {
            ferr() <<
                "Error, ambiguous definition for module \"" << name << "\":\n"
                "  " << source << "\n"
                "  " << otherSource << "\n";

            return false;
        }

        plan().sources().append(source);
    }

    return true;
}

void GlobbingStage::gatherSources() const
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
