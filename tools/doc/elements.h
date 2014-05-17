/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_ELEMENTS_H
#define FLUXDOC_ELEMENTS_H

#include <flux/File.h>
#include "Element.h"

namespace fluxdoc
{

using namespace flux;

class MarkupSyntax;

class TextElement: public Element
{
public:
	inline String text() const { return text_; }

	virtual void realize(const ByteArray *, Token *)
	{
		text_ = value("text");
	}

protected:
	TextElement(String className, YasonProtocol *protocol = 0)
		: Element(className, protocol)
	{}

	virtual void define()
	{
		insert("text", "");
	}

	String text_;
};

class TitleElement: public TextElement
{
public:
	static Ref<TitleElement> create() { return new TitleElement; }

protected:
	TitleElement(String className = "Title")
		: TextElement(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class AuthorElement: public Element
{
public:
	static Ref<AuthorElement> create() { return new AuthorElement; }

	String name() const { return name_; }
	String email() const { return email_; }

	virtual void realize(const ByteArray *text, Token *)
	{
		name_ = value("name");
		email_ = value("email");
	}

protected:
	AuthorElement(String className = "Author")
		: Element(className)
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

	String name_;
	String email_;
};

class ParagraphElement: public TextElement
{
public:
	static Ref<ParagraphElement> create() { return new ParagraphElement; }

protected:
	ParagraphElement(String className = "Paragraph")
		: TextElement(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class ListElement;

class ItemProtocol: public YasonProtocol
{
public:
	static Ref<ItemProtocol> create() { return new ItemProtocol; }

private:
	ItemProtocol() {}

	virtual int maxCount() const { return 1; }

	virtual YasonObject *lookup(String className)
	{
		if (className == "List") return listPrototype();
		return 0;
	}

	inline YasonObject *listPrototype()
	{
		if (!listPrototype_) listPrototype_ = createPrototype<ListElement>();
		return listPrototype_;
	}

	Ref<YasonObject> listPrototype_;
};

class ItemElement: public TextElement
{
public:
	static Ref<ItemElement> create() { return new ItemElement; }

protected:
	ItemElement(String className = "Item")
		: TextElement(className, ItemProtocol::create())
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class ListElement: public Element
{
public:
	static Ref<ListElement> create() { return new ListElement; }

	virtual void realize(const ByteArray *, Token *)
	{}

protected:
	ListElement(String className = "List")
		: Element(className, YasonProtocol::create())
	{}

	virtual void define()
	{
		protocol()->define<ItemElement>();
	}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class PathElement: public TextElement
{
public:
	inline String path() const { return path_; }
	inline String title() const { return title_; }

protected:
	PathElement(String className)
		: TextElement(className)
	{}

	virtual void define()
	{
		TextElement::define();
		insert("path", "");
		insert("title", "");
	}

	virtual void realize(const ByteArray *text, Token *objectToken)
	{
		TextElement::realize(text, objectToken);
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

class PartElement: public PathElement
{
public:
	static Ref<PartElement> create() { return new PartElement; }

protected:
	PartElement(String className = "Part")
		: PathElement(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class ImageElement: public PathElement
{
public:
	static Ref<ImageElement> create() { return new ImageElement; }

protected:
	ImageElement(String className = "Image")
		: PathElement(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

class CodeElement: public PathElement
{
public:
	static Ref<CodeElement> create() { return new CodeElement; }

protected:
	CodeElement(String className = "Code")
		: PathElement(className)
	{}

	virtual Ref<YasonObject> produce()
	{
		return create();
	}
};

} // namespace fluxdoc

#endif // FLUXDOC_ELEMENTS_H
