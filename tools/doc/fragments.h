/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_FRAGMENTS_H
#define FLUXDOC_FRAGMENTS_H

#include <flux/File.h>
#include "Fragment.h"

namespace fluxdoc
{

using namespace flux;

class MarkupSyntax;

class TextFragment: public Fragment
{
public:
	inline String text() const { return text_; }

protected:
	friend class MarkupSyntax;

	TextFragment(String className)
		: Fragment(className)
	{}

	virtual void define()
	{
		insert("text", "");
	}

	virtual void realize(const ByteArray *text, Token *)
	{
		text_ = value("text");
	}

	String text_;
};

class Title: public TextFragment
{
public:
	static Ref<Title> create() { return new Title; }

protected:
	Title(String className = "Title")
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class Author: public Fragment
{
public:
	static Ref<Author> create() { return new Author; }

	String name() const { return name_; }
	String email() const { return email_; }

protected:
	Author(String className = "Author")
		: Fragment(className)
	{}

	virtual void define()
	{
		insert("name", "");
		insert("email", "");
	}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}

	virtual void realize(const ByteArray *text, Token *)
	{
		name_ = value("name");
		email_ = value("email");
	}

	String name_;
	String email_;
};

class Heading: public TextFragment
{
public:
	static Ref<Heading> create() { return new Heading; }

protected:
	Heading(String className = "Heading")
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class Paragraph: public TextFragment
{
public:
	static Ref<Paragraph> create() { return new Paragraph; }

protected:
	Paragraph(String className = "Paragraph")
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class Item: public TextFragment
{
public:
	static Ref<Item> create() { return new Item; }

	inline int depth() const { return depth_; }

protected:
	friend class MarkupSyntax;

	Item(String className = "Item")
		: TextFragment(className)
	{}

	virtual void define()
	{
		TextFragment::define();
		insert("depth", 1);
	}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}

	virtual void realize(const ByteArray *text, Token *objectToken)
	{
		TextFragment::realize(text, objectToken);
		depth_ = value("depth");
	}

	int depth_;
};

class PathFragment: public TextFragment
{
public:
	inline String path() const { return path_; }
	inline String title() const { return title_; }

protected:
	PathFragment(String className)
		: TextFragment(className)
	{}

	virtual void define()
	{
		TextFragment::define();
		insert("path", "");
		insert("title", "");
	}

	virtual void realize(const ByteArray *text, Token *objectToken)
	{
		TextFragment::realize(text, objectToken);
		path_ = value("path");
		title_ = value("title");
		if (path_ != "") {
			try {
				File::open(path_);
			}
			catch (SystemError &ex) {
				int offset = valueToken(text, objectToken, "path")->i1();
				throw SemanticError(ex.message(), text, offset);
			}
		}
	}

	String path_;
	String title_;
};

class Part: public PathFragment
{
public:
	static Ref<Part> create() { return new Part; }

protected:
	Part(String className = "Part")
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class Image: public PathFragment
{
public:
	static Ref<Image> create() { return new Image; }

protected:
	Image(String className = "Image")
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class Code: public PathFragment
{
public:
	static Ref<Code> create() { return new Code; }

protected:
	Code(String className = "Code")
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

} // namespace fluxdoc

#endif // FLUXDOC_FRAGMENTS_H
