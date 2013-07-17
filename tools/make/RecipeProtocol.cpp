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
	GenericPrototype(const String &className)
		: YasonObject(className)
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

		insert("clean", false);
		insert("install", false);
		insert("uninstall", false);
	}
};

class ApplicationPrototype: public GenericPrototype
{
public:
	static Ref<YasonObject> create() {
		return new ApplicationPrototype("Application");
	}

protected:
	ApplicationPrototype(const String &className)
		: GenericPrototype(className)
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
