/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_AUTHORELEMENT_H
#define FLUXDOC_AUTHORELEMENT_H

namespace flux {
namespace doc {

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

	virtual Ref<MetaObject> produce()
	{
		return create();
	}

	String name_;
	String email_;
};

}} // namespace flux::doc

#endif // FLUXDOC_AUTHORELEMENT_H
