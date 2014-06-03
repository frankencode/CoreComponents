/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_STYLE_H
#define FLUXTOKI_STYLE_H

#include <flux/yason.h>
#include <flux/Color.h>

namespace fluxtoki
{

using namespace flux;

class Palette;

class Style: public YasonObject
{
public:
	static Ref<Style> create() { return new Style; }

	inline String name() const { return name_; }
	inline Color ink() const { return ink_; }
	inline Color paper() const { return paper_; }
	inline Ref<StringList> font() const { return font_; }
	inline bool bold() const { return bold_; }
	inline bool italic() const { return italic_; }

private:
	Style() {}
	virtual Ref<YasonObject> produce();
	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);

	String name_;
	Color ink_;
	Color paper_;
	Ref<StringList> font_;
	bool bold_;
	bool italic_;
};

} // namespace fluxtoki

#endif // FLUXTOKI_STYLE_H
