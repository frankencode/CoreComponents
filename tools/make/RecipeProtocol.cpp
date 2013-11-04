/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "RecipeProtocol.h"

namespace fmake
{

class GenericPrototype: public YasonObject
{
protected:
	GenericPrototype(const String &className, YasonProtocol *protocol = 0)
		: YasonObject(className, protocol)
	{
		insert("use", StringList::create());
		insert("debug", false);
		insert("release", false);
		insert("static", false);
		insert("simulate", false);
		insert("blindfold", false);
		insert("bootstrap", false);
		insert("install", false);
		insert("test", false);
		insert("test-run", false);
		insert("verbose", false);
		insert("optimize-speed", false);
		insert("optimize-size", false);
		insert("speed-optimization-level", -1);
		insert("size-optimization-level", -1);
		insert("include-path", Ref<StringList>());
		insert("link-path", Ref<StringList>());
		insert("link", Ref<StringList>());
		insert("prefix", "/usr");

		insert("compile-flags", Ref<StringList>());
		insert("link-flags", Ref<StringList>());
		insert("debug-compile-flags", Ref<StringList>());
		insert("release-compile-flags", Ref<StringList>());
		insert("debug-link-flags", Ref<StringList>());
		insert("release-link-flags", Ref<StringList>());

		insert("compiler", "");
		insert("concurrency", -1);

		insert("clean", false);
		insert("install", false);
		insert("uninstall", false);
	}
};

class SystemPrerequisitePrototype: public YasonObject
{
public:
	static Ref<YasonObject> create() {
		return new SystemPrerequisitePrototype("SystemPrerequisite");
	}

protected:
	SystemPrerequisitePrototype(const String &className)
		: YasonObject(className)
	{
		insert("name", "");
		insert("value", "");
		insert("description", "");
		insert("optional", false);
		insert("include-path", StringList::create());
		insert("include-test", StringList::create());
		insert("link-path", StringList::create());
		insert("link-test", StringList::create());
		insert("link", StringList::create());

		insert("compile-flags", StringList::create());
		insert("debug-compile-flags", StringList::create());
		insert("release-compile-flags", StringList::create());
	}
};

class ApplicationPrototype: public GenericPrototype
{
public:
	static Ref<YasonObject> create() {
		return new ApplicationPrototype("Application");
	}

protected:
	static Ref<YasonProtocol> createProtocol() {
		Ref<YasonProtocol> protocol = YasonProtocol::create();
		protocol->add<SystemPrerequisitePrototype>();
		return protocol;
	}

	ApplicationPrototype(const String &className)
		: GenericPrototype(className, createProtocol())
	{
		insert("name", "");
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
	LibraryPrototype(const String &className)
		: ApplicationPrototype(className)
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

class PackagePrototype: public GenericPrototype
{
public:
	static Ref<YasonObject> create(const String &className = "Package") {
		return new PackagePrototype(className);
	}

protected:
	PackagePrototype(const String &className)
		: GenericPrototype(className)
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

} // namespace fmake
