/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_STYLE_H
#define FLUXTOKI_STYLE_H

#include <flux/Color.h>
#include <flux/yason.h>

namespace flux {
namespace toki {

class Palette;

class Style: public MetaObject
{
public:
	static Ref<Style> create() { return new Style; }

	inline String ruleName() const { return ruleName_; }
	inline int rule() const { return rule_; }
	inline Color ink() const { return ink_; }
	inline Color paper() const { return paper_; }
	inline bool bold() const { return bold_; }
	inline bool italic() const { return italic_; }

private:
	friend class Palette;

	Style() {}
	virtual Ref<MetaObject> produce();
	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);

	String ruleName_;
	int rule_;
	Color ink_;
	Color paper_;
	bool bold_;
	bool italic_;
};

}} // namespace flux::toki

#endif // FLUXTOKI_STYLE_H
