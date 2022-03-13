/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/RecipeProtocol>
#include <cc/MetaProtocolState>
#include <cc/Version>

namespace cc::build {

class BuildParametersPrototype: public MetaPrototype
{
public:
    BuildParametersPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        MetaPrototype{className, protocol}
    {
        insert("compiler", "");
        insert("optimize", "");
        insert("static", false);

        insert("include-paths", List<String>{});
        insert("library-paths", List<String>{});
        insert("link", List<String>{});

        insert("compile-flags", List<String>{});
        insert("link-flags", List<String>{});
    }
};

class SpecificBuildParametersPrototype: public BuildParametersPrototype
{
public:
    SpecificBuildParametersPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        BuildParametersPrototype{
            className,
            [=]{
                MetaProtocol target { protocol };

                const char *paramClasses[] = {
                    "Debug", "Release",
                    "Linux", "OpenBSD", "FreeBSD", "NetBSD", "DragonFly",
                    "MacOSX", "Solaris", "Mach", "HPUX"
                };

                for (const char *className: paramClasses) {
                    target.define(BuildParametersPrototype{className});
                }

                return target;
            }()
        }
    {}
};

class UserStop: public MetaPrototype
{
public:
    UserStop(const String &className):
        MetaPrototype{className}
    {
        insert("execute", "");
    }
};

class BuildOptionsPrototype: public SpecificBuildParametersPrototype
{
protected:
    BuildOptionsPrototype(const String &className, const MetaProtocol &protocol = MetaProtocol{}):
        SpecificBuildParametersPrototype{
            className,
            [=]{
                MetaProtocol target = protocol;

                const char *stopClasses[] = {
                    "PreBuild",     "PostBuild",
                    "PreClean",     "PostClean",
                    "PreInstall",   "PostInstall",
                    "PreUninstall", "PostUninstall",
                };

                for (const char *className: stopClasses) {
                    target.define(UserStop{className});
                }

                return target;
            }()
        }
    {
        insert("use", List<String>{});
        insert("root", "/");
        insert("prefix", "");

        insert("debug", false);
        insert("release", false);

        insert("configure", false);
        insert("configure-list", false);
        insert("prepare", false);
        insert("preprocess", false);
        insert("clean", false);
        insert("install", false);
        insert("uninstall", false);
        insert("test", false);
        insert("test-run", false);
        insert("test-report", false);
        insert("test-args", "");

        insert("verbose", false);
        insert("jobs", -1);
        insert("test-run-jobs", -1);

        insert("simulate", false);
        insert("blindfold", false);
        insert("bootstrap", false);

        insert("query", "");
        insert("query-all", false);

        insert("pkg-config", false);
    }
};

class SystemPrerequisitePrototype: public SpecificBuildParametersPrototype
{
public:
    SystemPrerequisitePrototype():
        SpecificBuildParametersPrototype{"Dependency"}
    {
        insert("name", "");
        insert("optional", false);
        insert("cascade", false);

        insert("compile-flags-configure", "");
        insert("link-flags-configure", "");

        insert("version-configure", "");
        insert("version-min", Version{});
        insert("version-max", Version{});

        insert("probe", "");
        insert("configure", "");
    }
};

class PredicatePrototype;

class PredicateMetaProtocol: public MetaProtocol
{
public:
    PredicateMetaProtocol(const PredicatePrototype &prototype);

private:
    struct State;
};

class PredicatePrototype: public MetaPrototype
{
public:
    PredicatePrototype():
        MetaPrototype{"Predicate", PredicateMetaProtocol{*this}}
    {
        insert("source", List<String>{});
        insert("target", "");
        insert("create", "");
        insert("update", "");
        insert("remove", "");
        insert("clean", "");
    }
};

struct PredicateMetaProtocol::State: public MetaProtocol::State
{
    State(const PredicatePrototype &prototype):
        prototype_{prototype}
    {}

    bool lookup(const String &className, Out<MetaPrototype> prototype) const override
    {
        if (className == prototype_.className()) {
            prototype = prototype_;
            return true;
        }
        return false;
    }

    const PredicatePrototype &prototype_;
};

PredicateMetaProtocol::PredicateMetaProtocol(const PredicatePrototype &prototype):
    MetaProtocol{new State{prototype}}
{}

class ApplicationPrototype: public BuildOptionsPrototype
{
public:
    ApplicationPrototype(const String &className = "Application", const MetaProtocol &protocol = MetaProtocol{}):
        BuildOptionsPrototype{
            className,
            [=]{
                MetaProtocol target = protocol;
                target.define(SystemPrerequisitePrototype{});
                target.define(PredicatePrototype{});
                return target;
            }()
        }
    {
        insert("name", "");
        insert("description", "");
        insert("alias", List<String>{});
        insert("depends", List<String>{});
        insert("source", List<String>{"*.(cc|cpp|cxx|c)"});
        insert("bundle", List<String>{});
        insert("version", Version{});
    }
};

class TestPrototype: public ApplicationPrototype
{
public:
    TestPrototype(const String &className = "Test"):
        ApplicationPrototype{className}
    {
        remove("prefix");
        remove("install");
    }
};

class LibraryPrototype: public ApplicationPrototype
{
public:
    LibraryPrototype():
        ApplicationPrototype{"Library"}
    {}
};

class PluginPrototype: public ApplicationPrototype
{
public:
    PluginPrototype():
        ApplicationPrototype{"Plugin"}
    {
        insert("extend", "");
        insert("group", "");
    }
};

class ToolsPrototype: public ApplicationPrototype
{
public:
    ToolsPrototype():
        ApplicationPrototype{"Tools"}
    {
        remove("name");
    }
};

class TestsPrototype: public TestPrototype
{
public:
    TestsPrototype():
        TestPrototype{"Tests"}
    {
        remove("name");
    }
};

class PackagePrototype: public ApplicationPrototype
{
public:
    PackagePrototype():
        ApplicationPrototype{"Package"}
    {
        // insert("version", Version{});
        insert("include", List<String>{}); // FIXME: confusing
    }
};

struct RecipeProtocol::State: public MetaProtocol::State
{};

RecipeProtocol::RecipeProtocol()
{
    initOnce<State>();

    define(ApplicationPrototype{});
    define(TestPrototype{});
    define(LibraryPrototype{});
    define(PluginPrototype{});
    define(ToolsPrototype{});
    define(TestsPrototype{});
    define(PackagePrototype{});
}

} // namespace cc::build
