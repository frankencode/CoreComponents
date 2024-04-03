/*
 * Copyright (C) 2022-2024 Frank Mertens.
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
#include <cc/debugging>

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

    if (!(plan().options() & BuildOption::Cody)) {
        return true;
    }

    for (const String &source: DirWalk{plan().importPath(), DirWalk::FilesOnly})
    {
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

    bool autoSubDir = true;
    for (const String &pattern: patternList) {
        if (pattern.contains('/')) {
            autoSubDir = false;
            break;
        }
    }

    if (autoSubDir) {
        List<String> dirs;
        dirs.append(plan().projectPath());
        for (const String &path: DirWalk{plan().projectPath(), DirWalk::DirsOnly}) {
            List<String> parts = path.copy(plan().projectPath().count(), path.count()).split('/');
            if (parts.contains("import") || parts.contains("include") || parts.contains("probe")) continue;
            bool hidden = false;
            for (const String &part: parts) {
                if (part.startsWith('.')) {
                    hidden = true;
                    break;
                }
            }
            if (hidden) continue;
            dirs.append(path);
        }

        if (!(plan().options() & BuildOption::Bootstrap)) {
            dirs.append(plan().prestagePath());
        }

        for (const String &dir: dirs) {
            for (const String &pattern: patternList) {
                for (const String &globbedPath: Glob{dir / pattern}) {
                    appendPath(globbedPath, &sources);
                }
            }
        }
    }
    else {
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
