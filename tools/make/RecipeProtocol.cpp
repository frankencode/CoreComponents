/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "RecipeProtocol.h"

namespace fluxmake
{

class BuildParametersPrototype: public YasonObject
{
public:
	static Ref<BuildParametersPrototype> create(const String &className) {
		return new BuildParametersPrototype(className);
	}

protected:
	BuildParametersPrototype(const String &className, YasonProtocol *protocol = 0)
		: YasonObject(className, protocol)
	{
		insert("compiler", "");
		insert("optimize", "");
		insert("static", false);

		insert("include-path", Ref<StringList>());
		insert("link-path", Ref<StringList>());
		insert("link", Ref<StringList>());

		insert("compile-flags", Ref<StringList>());
		insert("link-flags", Ref<StringList>());
	}
};

class BuildOptionsPrototype: public BuildParametersPrototype
{
protected:
	static Ref<YasonProtocol> createProtocol(YasonProtocol *protocol) {
		Ref<YasonProtocol> newProtocol;
		if (!protocol) protocol = newProtocol = YasonProtocol::create();
		protocol->add<BuildParametersPrototype>("Debug");
		protocol->add<BuildParametersPrototype>("Release");
		return protocol;
	}

	BuildOptionsPrototype(const String &className, YasonProtocol *protocol = 0)
		: BuildParametersPrototype(className, createProtocol(protocol))
	{
		insert("use", StringList::create());
		insert("prefix", "/usr");

		insert("debug", false);
		insert("release", false);

		insert("simulate", false);
		insert("blindfold", false);
		insert("bootstrap", false);
		insert("install", false);
		insert("test", false);
		insert("test-run", false);
		insert("clean", false);
		insert("install", false);
		insert("uninstall", false);

		insert("verbose", false);
		insert("concurrency", -1);
	}
};

class SystemPrerequisitePrototype: public BuildParametersPrototype
{
public:
	static Ref<YasonObject> create() {
		return new SystemPrerequisitePrototype("SystemPrerequisite");
	}

protected:
	static Ref<YasonProtocol> createProtocol() {
		Ref<YasonProtocol> protocol = YasonProtocol::create();
		protocol->add<BuildParametersPrototype>("Debug");
		protocol->add<BuildParametersPrototype>("Release");
		return protocol;
	}

	SystemPrerequisitePrototype(const String &className)
		: BuildParametersPrototype(className, createProtocol())
	{
		insert("name", "");
		insert("value", "");
		insert("description", "");
		insert("optional", false);

		insert("include-test", StringList::create());
		insert("link-test", StringList::create());
	}
};

class ApplicationPrototype: public BuildOptionsPrototype
{
public:
	static Ref<YasonObject> create() {
		return new ApplicationPrototype("Application");
	}

protected:
	static Ref<YasonProtocol> createProtocol(YasonProtocol *protocol) {
		Ref<YasonProtocol> newProtocol;
		if (!protocol) protocol = newProtocol = YasonProtocol::create();
		protocol->add<SystemPrerequisitePrototype>();
		return protocol;
	}

	ApplicationPrototype(const String &className, YasonProtocol *protocol = 0)
		: BuildOptionsPrototype(className, createProtocol(protocol))
	{
		insert("name", "");
		insert("alias", StringList::create());
		insert("source", StringList::create());
	}
};

class LibraryPrototype: public ApplicationPrototype
{
public:
	static Ref<YasonObject> create(const String &className = "Library") {
		return new LibraryPrototype(className);
	}

protected:
	static Ref<YasonProtocol> createProtocol() {
		Ref<YasonProtocol> protocol = YasonProtocol::create();
		protocol->add<BuildParametersPrototype>("Usage");
		return protocol;
	}

	LibraryPrototype(const String &className)
		: ApplicationPrototype(className, createProtocol())
	{
		insert("version", "0.0.1");
		insert("header", StringList::create());
	}
};

class ToolsPrototype: public ApplicationPrototype
{
public:
	static Ref<YasonObject> create(const String &className = "Tools") {
		return new ToolsPrototype(className);
	}

protected:
	ToolsPrototype(const String &className)
		: ApplicationPrototype(className)
	{
		remove("name");
	}
};

class TestsPrototype: public ToolsPrototype
{
public:
	static Ref<YasonObject> create(const String &className = "Tests") {
		return new TestsPrototype(className);
	}

protected:
	TestsPrototype(const String &className)
		: ToolsPrototype(className)
	{
		remove("prefix");
		remove("install");
	}
};

class PackagePrototype: public BuildOptionsPrototype
{
public:
	static Ref<YasonObject> create(const String &className = "Package") {
		return new PackagePrototype(className);
	}

protected:
	PackagePrototype(const String &className)
		: BuildOptionsPrototype(className)
	{}
};

RecipeProtocol::RecipeProtocol()
{
	add<ApplicationPrototype>();
	add<LibraryPrototype>();
	add<ToolsPrototype>();
	add<TestsPrototype>();
	add<PackagePrototype>();
}

} // namespace fluxmake
