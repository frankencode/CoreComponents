/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
