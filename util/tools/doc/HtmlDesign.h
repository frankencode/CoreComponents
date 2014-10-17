/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_HTMLDESIGN_H
#define FLUXDOC_HTMLDESIGN_H

#include <flux/toki/Theme>
#include "Design.h"

namespace flux {
namespace doc {

class HtmlDesign: public Design
{
public:
    static Ref<HtmlDesign> create() { return new HtmlDesign; }

    virtual void realize(const ByteArray *text, Token *objectToken);

    inline toki::Theme *tokiTheme() const { return tokiTheme_; }

private:
    HtmlDesign();
    virtual void define();
    virtual Ref<MetaObject> produce();

    Ref<toki::Theme> tokiTheme_;
};

}} // namespace flux::doc

#endif // FLUXDOC_HTMLDESIGN_H
