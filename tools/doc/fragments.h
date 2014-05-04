#ifndef FLUXDOC_FRAGMENTS_H
#define FLUXDOC_FRAGMENTS_H

#include "Fragment.h"

namespace fluxdoc
{

using namespace flux;

class TextFragment: public Fragment
{
public:
	inline String text() const { return text_; }

protected:
	TextFragment(String className)
		: Fragment(className)
	{
		insert("text", "");
	}

	virtual void realize(Token *)
	{
		text_ = value("text");
	}

private:
	String text_;
};

class Title: public TextFragment
{
public:
	static Ref<Title> create(String className = "Title") {
		return new Title(className);
	}

protected:
	Title(String className)
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class Author: public Fragment
{
public:
	static Ref<Author> create(String className = "Author") {
		return new Author(className);
	}

	String name() const { return name_; }
	String email() const { return email_; }

protected:
	Author(String className)
		: Fragment(className)
	{
		insert("name", "");
		insert("email", "");
	}

	virtual Ref<YasonObject> produce() {
		return create();
	}

	virtual void realize(Token *) {
		name_ = value("name");
		email_ = value("email");
	}

private:
	String name_;
	String email_;
};

class Heading: public TextFragment
{
public:
	static Ref<Heading> create(String className = "Heading") {
		return new Heading(className);
	}

protected:
	Heading(String className)
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class Paragraph: public TextFragment
{
public:
	static Ref<Paragraph> create(String className = "Paragraph") {
		return new Paragraph(className);
	}

protected:
	Paragraph(String className)
		: TextFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class Item: public TextFragment
{
public:
	static Ref<Item> create(String className = "Item") {
		return new Item(className);
	}

	inline int depth() const { return depth_; }

protected:
	Item(String className)
		: TextFragment(className)
	{
		insert("depth", 1);
	}

	virtual Ref<YasonObject> produce() {
		return create();
	}

	virtual void realize(Token *objectToken) {
		TextFragment::realize(objectToken);
		depth_ = value("depth");
	}

private:
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
	{
		insert("path", "");
		insert("title", "");
	}

	virtual void realize(Token *objectToken) {
		TextFragment::realize(objectToken);
		path_ = value("path");
		title_ = value("title");
	}

private:
	String path_;
	String title_;
};

class Part: public PathFragment
{
public:
	static Ref<Part> create(String className = "Part") {
		return new Part(className);
	}

protected:
	Part(String className)
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class Image: public PathFragment
{
public:
	static Ref<Image> create(String className = "Image") {
		return new Image(className);
	}

protected:
	Image(String className)
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class Code: public PathFragment
{
public:
	static Ref<Code> create(String className = "Code") {
		return new Code(className);
	}

protected:
	Code(String className)
		: PathFragment(className)
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

} // namespace fluxdoc

#endif // FLUXDOC_FRAGMENTS_H
