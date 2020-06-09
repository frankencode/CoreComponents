/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include "BuildPlan.h"
#include "ConfigureShell.h"
#include "BuildParameters.h"

namespace ccbuild {

void BuildParameters::read(const MetaObject *object, const BuildPlan *plan)
{
    compiler_ = object->value("compiler");
    optimize_ = object->value("optimize");
    linkStatic_ = object->value("static");

    includePaths_ = Variant::cast<StringList>(object->value("include-paths"));
    libraryPaths_ = Variant::cast<StringList>(object->value("library-paths"));
    libraries_ = Variant::cast<StringList>(object->value("link"));

    for (String &path: includePaths_) {
        if (path->isRelativePath())
            path = path->absolutePathRelativeTo(plan->projectPath());
    }

    customCompileFlags_ = Variant::cast<StringList>(object->value("compile-flags"));
    customLinkFlags_ = Variant::cast<StringList>(object->value("link-flags"));

    for (const MetaObject *child: object->children()) {
        if (child->className() == "Debug") {
            if (plan->options() & BuildPlan::Debug) readSpecific(child);
        }
        else if (child->className() == "Release") {
            if (plan->options() & BuildPlan::Release) readSpecific(child);
        }
        else {
            #ifdef __linux
            if (child->className() == "Linux") readSpecific(child);
            #endif
            #ifdef __OpenBSD__
            if (child->className() == "OpenBSD") readSpecific(child);
            #endif
            #ifdef __FreeBSD__
            if (child->className() == "FreeBSD") readSpecific(child);
            #endif
            #ifdef __NetBSD__
            if (child->className() == "NetBSD") readSpecific(child);
            #endif
            #ifdef __DragonFly__
            if (child->className() == "DragonFly") readSpecific(child);
            #endif
            #ifdef __APPLE__
            if (child->className() == "MacOSX") readSpecific(child);
            #endif
            #ifdef __sun
            if (child->className() == "Solaris") readSpecific(child);
            #endif
            #ifdef __MACH__
            if (child->className() == "Mach") readSpecific(child);
            #endif
            #ifdef __hpux
            if (child->className() == "HPUX") readSpecific(child);
            #endif
        }
    }
}

void BuildParameters::readSpecific(const MetaObject *object)
{
    Ref<BuildParameters> specific = BuildParameters::create();
    specific->read(object);
    readSpecific(specific);
}

void BuildParameters::readSpecific(BuildParameters *specific)
{
    if (specific->compiler() != "") compiler_ = specific->compiler();
    if (specific->optimize() != "") optimize_ = specific->optimize();
    if (specific->linkStatic()) linkStatic_ = true;

    includePaths_->appendList(specific->includePaths());
    libraryPaths_->appendList(specific->libraryPaths());
    libraries_->appendList(specific->libraries());

    customCompileFlags_->appendList(specific->customCompileFlags());
    customLinkFlags_->appendList(specific->customLinkFlags());
}

} // namespace ccbuild
