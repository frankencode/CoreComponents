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
#include <cc/Dir>
#include <cc/DirWalker>
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

    if (!Dir::exists(interfacePath)) return true;

    String path;
    bool isDir = false;

    for (DirWalker walker{interfacePath}; walker.read(&path, &isDir) ;) {
        if (isDir) continue;
        String suffix = path.fileSuffix();
        String moduleName = path.copy(interfacePath.count(), path.count() - suffix.count() - (suffix.count() > 0));
        moduleName.replace('/', '.');
        moduleName.replace("__", ".");
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
        plan().sources() = sources;
    }
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

void GlobbingStage::appendPath(const String &path, Out<List<String>> sources)
{
    if(
        !path.endsWith(".lump.cc") &&
        !path.endsWith(".lump.c")
    )
        sources->append(path);
}

} // namespace cc::build
