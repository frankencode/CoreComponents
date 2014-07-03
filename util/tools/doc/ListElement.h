/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_LISTELEMENT_H
#define FLUXDOC_LISTELEMENT_H

#include "TextElement.h"

namespace flux {
namespace doc {

class ListElement;

class ItemProtocol: public MetaProtocol
{
public:
	static Ref<ItemProtocol> create() { return new ItemProtocol; }

private:
	ItemProtocol() { maxCount(1); }

	virtual MetaObject *lookup(String className) const
	{
		if (className == "List") return listPrototype();
		return 0;
	}

	inline MetaObject *listPrototype() const
	{
		if (!listPrototype_) listPrototype_ = createPrototype<ListElement>();
		return listPrototype_;
	}

	mutable Ref<MetaObject> listPrototype_;
};

class ItemElement: public TextElement
{
public:
	static Ref<ItemElement> create() { return new ItemElement; }

protected:
	ItemElement(String className = "Item")
		: TextElement(className, ItemProtocol::create())
	{}

	virtual Ref<MetaObject> produce()
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
		: Element(className, MetaProtocol::create())
	{}

	virtual void define()
	{
		protocol()->define<ItemElement>();
	}

	virtual Ref<MetaObject> produce()
	{
		return create();
	}
};


}} // namespace flux::doc

#endif // FLUXDOC_LISTELEMENT_H
