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

    {
        String sourcePrefix = BuildMap{}.commonPrefix();
        if (sourcePrefix == "") sourcePrefix = plan().projectPath();
        else sourcePrefix = sourcePrefix.canonicalPath();
        plan().sourcePrefix() = sourcePrefix;
    }

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
        plan().sources() = sources;
    }

    const String currentCompileCommand = toolChain().compileCommand(plan(), "%.cc", "%.o");
    const String previousCompileCommand = File::load(plan().previousCompileCommandPath());
    const bool compileCommandChanged = currentCompileCommand != previousCompileCommand;
    File::save(plan().previousCompileCommandPath(), currentCompileCommand);

    for (const String &source: plan().sources()) {
        const String objectFilePath = toolChain().objectFilePath(plan(), source);
        const String compileCommand = toolChain().compileCommand(plan(), source, objectFilePath);
        List<String> previousDependencyPaths;
        if (toolChain().readObjectDependencies(objectFilePath, &previousDependencyPaths)) {
            assert(previousDependencyPaths.at(0) == source);
            bool dirty = compileCommandChanged;
            do {
                if (dirty) break;
                FileInfo objectFileInfo = shell().fileStatus(objectFilePath);
                if (!objectFileInfo) { dirty = true; break; }
                for (const String &previousDependencyPath: previousDependencyPaths) {
                    FileInfo previousDependencyInfo = shell().fileStatus(previousDependencyPath);
                    if (!previousDependencyInfo) {
                        dirty = true;
                        break;
                    }
                    if (objectFileInfo.lastModified() < previousDependencyInfo.lastModified()) {
                        dirty = true;
                        break;
                    }
                }
            }
            while (false);
            plan().objectFiles().emplaceBack(compileCommand, objectFilePath, previousDependencyPaths, dirty);
        }
        else {
            plan().objectFiles().emplaceBack(compileCommand, objectFilePath, List<String>{source}, true);
        }
    }

    for (BuildPlan &prerequisite: plan().prerequisites()) {
        if (!prerequisite.globbingStage().run()) {
            return success_ = false;
        }
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
