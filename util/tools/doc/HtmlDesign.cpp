/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/toki/Theme>
#include "HtmlDesign.h"

namespace flux {
namespace doc {

HtmlDesign::HtmlDesign()
    : Design("Html")
{}

void HtmlDesign::realize(const ByteArray *text, Token *objectToken)
{
    Design::realize(text, objectToken);
    String themePath = value("toki-theme");
    try {
        tokiTheme_ = toki::Theme::load(themePath);
    }
    catch (Exception &ex) {
        throw SemanticError(ex.message(), text, valueToken(text, objectToken, "toki-theme")->i1());
    }
}

void HtmlDesign::define()
{
    Design::define();
    insert("toki-theme", "ClassicWhite");
}

Ref<MetaObject> HtmlDesign::produce()
{
    return HtmlDesign::create();
}

}} // namespace flux::doc
