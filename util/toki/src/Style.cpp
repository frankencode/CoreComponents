/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Style"

namespace flux {
namespace toki {

Ref<MetaObject> Style::produce()
{
	return Style::create();
}

void Style::define()
{
	className("Style");
	insert("rule", "");
	insert("ink", Color());
	insert("paper", Color());
	insert("bold", false);
	insert("italic", false);
}

void Style::realize(const ByteArray *text, Token *objectToken)
{
	ruleName_ = value("rule");
	ink_ = value("ink");
	paper_ = value("paper");
	bold_ = value("bold");
	italic_ = value("italic");
}

}} // namespace flux::toki
