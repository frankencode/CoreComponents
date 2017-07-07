/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include "RecipeProtocol.h"

namespace ccbuild {

class BuildParametersPrototype: public MetaObject
{
public:
    static Ref<BuildParametersPrototype> create(const String &className) {
        return new BuildParametersPrototype(className);
    }

protected:
    BuildParametersPrototype(const String &className, MetaProtocol *protocol = 0):
        MetaObject(className, protocol)
    {
        insert("compiler", "");
        insert("optimize", "");
        insert("static", false);

        insert("include-paths", Ref<StringList>());
        insert("library-paths", Ref<StringList>());
        insert("link", Ref<StringList>());

        insert("compile-flags", Ref<StringList>());
        insert("link-flags", Ref<StringList>());
    }
};

class SpecificBuildParametersPrototype: public BuildParametersPrototype
{
public:
    static Ref<SpecificBuildParametersPrototype> create(const String &className) {
        return new SpecificBuildParametersPrototype(className);
    }

protected:
    static Ref<MetaProtocol> createProtocol(MetaProtocol *protocol) {
        Ref<MetaProtocol> newProtocol;
        if (!protocol) protocol = newProtocol = MetaProtocol::create();
        const char *paramClasses[] = {
            "Debug", "Release",
            "Linux", "OpenBSD", "FreeBSD", "NetBSD", "DragonFly",
            "MacOSX", "Solaris", "Mach", "HPUX"
        };
        for (int i = 0, n = sizeof(paramClasses) / sizeof(paramClasses[0]); i < n; ++i)
            protocol->define<BuildParametersPrototype>(paramClasses[i]);
        return protocol;
    }

    SpecificBuildParametersPrototype(const String &className, MetaProtocol *protocol = 0):
        BuildParametersPrototype(className, createProtocol(protocol))
    {}
};

class BuildOptionsPrototype: public SpecificBuildParametersPrototype
{
protected:
    BuildOptionsPrototype(const String &className, MetaProtocol *protocol = 0):
        SpecificBuildParametersPrototype(className, protocol)
    {
        insert("use", StringList::create());
        insert("choice", StringList::create());
        insert("choice-configure", "");
        insert("root", "/");
        insert("prefix", "/usr");

        insert("debug", false);
        insert("release", false);

        insert("simulate", false);
        insert("blindfold", false);
        insert("bootstrap", false);
        insert("test", false);
        insert("test-run", false);
        insert("test-report", false);
        insert("test-args", "");
        insert("clean", false);
        insert("install", false);
        insert("uninstall", false);

        insert("verbose", false);
        insert("configure", false);
        insert("select", StringList::create());
        insert("jobs", -1);
        insert("test-run-jobs", -1);
    }
};

class SystemPrerequisitePrototype: public SpecificBuildParametersPrototype
{
public:
    static Ref<MetaObject> create() {
        return new SystemPrerequisitePrototype("Dependency");
    }

protected:
    SystemPrerequisitePrototype(const String &className):
        SpecificBuildParametersPrototype(className)
    {
        insert("name", "");
        insert("value", "");
        insert("description", "");
        insert("optional", false);

        insert("include-paths-configure", "");
        insert("library-paths-configure", "");

        insert("compile-flags-configure", "");
        insert("link-flags-configure", "");

        insert("version-configure", "");
        insert("version-min", Version());
        insert("version-max", Version());

        insert("include-test", StringList::create());
    }
};

class PredicatePrototype: public MetaObject
{
public:
    static Ref<MetaObject> create() {
        return new PredicatePrototype("Predicate");
    }

protected:
    PredicatePrototype(const String &className):
        MetaObject(className)
    {
        insert("source", StringList::create());
        insert("target", "");
        insert("create", "");
        insert("update", "");
        insert("remove", "");
    }
};

class ApplicationPrototype: public BuildOptionsPrototype
{
public:
    static Ref<MetaObject> create() {
        return new ApplicationPrototype("Application");
    }

protected:
    static Ref<MetaProtocol> createProtocol(MetaProtocol *protocol) {
        Ref<MetaProtocol> newProtocol;
        if (!protocol) protocol = newProtocol = MetaProtocol::create();
        protocol->define<SystemPrerequisitePrototype>();
        protocol->define<PredicatePrototype>();
        return protocol;
    }

    ApplicationPrototype(const String &className, MetaProtocol *protocol = 0):
        BuildOptionsPrototype(className, createProtocol(protocol))
    {
        insert("name", "");
        insert("alias", StringList::create());
        insert("source", StringList::create());
        insert("bundle", StringList::create());
        insert("version", Version());
    }
};

class TestPrototype: public ApplicationPrototype
{
public:
    static Ref<MetaObject> create() {
        return new TestPrototype("Test");
    }

protected:
    TestPrototype(const String &className):
        ApplicationPrototype(className)
    {
        remove("prefix");
        remove("install");
    }
};

class LibraryPrototype: public ApplicationPrototype
{
public:
    static Ref<MetaObject> create(const String &className = "Library") {
        return new LibraryPrototype(className);
    }

protected:
    static Ref<MetaProtocol> createProtocol() {
        Ref<MetaProtocol> protocol = MetaProtocol::create();
        protocol->define<SpecificBuildParametersPrototype>("Usage");
        return protocol;
    }

    LibraryPrototype(const String &className):
        ApplicationPrototype(className, createProtocol())
    {}
};

class ToolsPrototype: public ApplicationPrototype
{
public:
    static Ref<MetaObject> create(const String &className = "Tools") {
        return new ToolsPrototype(className);
    }

protected:
    ToolsPrototype(const String &className):
        ApplicationPrototype(className)
    {
        remove("name");
    }
};

class TestsPrototype: public TestPrototype
{
public:
    static Ref<MetaObject> create(const String &className = "Tests") {
        return new TestsPrototype(className);
    }

protected:
    TestsPrototype(const String &className):
        TestPrototype(className)
    {
        remove("name");
    }
};

class PackagePrototype: public BuildOptionsPrototype
{
public:
    static Ref<MetaObject> create(const String &className = "Package") {
        return new PackagePrototype(className);
    }

protected:
    PackagePrototype(const String &className):
        BuildOptionsPrototype(className)
    {
        insert("version", Version());
    }
};

RecipeProtocol::RecipeProtocol()
{
    define<ApplicationPrototype>();
    define<TestPrototype>();
    define<LibraryPrototype>();
    define<ToolsPrototype>();
    define<TestsPrototype>();
    define<PackagePrototype>();
}

const RecipeProtocol *recipeProtocol() { return Singleton<RecipeProtocol>::instance(); }

} // namespace ccbuild
