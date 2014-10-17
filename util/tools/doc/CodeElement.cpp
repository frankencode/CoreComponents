/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/toki/Registry>
#include "CodeElement.h"

namespace flux {
namespace doc {

CodeElement::CodeElement(String className)
    : PathElement(className)
{}

void CodeElement::define()
{
    PathElement::define();
    insert("language", "");
}

void CodeElement::realize(const ByteArray *text, Token *objectToken)
{
    PathElement::realize(text, objectToken);

    String name = value("language");
    if (name != "") {
        toki::Language *language = 0;
        if (toki::registry()->lookupLanguageByName(name, &language)) {
            language_ = language;
        }
        else {
            int n = toki::registry()->languageCount();
            name = name->downcase();
            for (int i = 0; i < n; ++i) {
                toki::Language *candidate = toki::registry()->languageAt(i);
                if (candidate->displayName()->downcase() == name) {
                    language_ = candidate;
                    break;
                }
            }
            if (!language_) {
                int offset = valueToken(text, objectToken, "language")->i1();
                ferr() << SemanticError(Format("Unknown language \"%%\", falling back to \"plaintext\"") << name, text, offset) << nl;
            }
        }
    }
    if (!language_) {
        toki::Language *language = 0;
        toki::registry()->lookupLanguageByName("plaintext", &language);
        language_ = language;
    }

    state_ = language_->highlightingSyntax()->match(text_);
    if (!state_->valid()) {
        int offset = valueToken(text, objectToken, (path_ != "") ? "path" : "text")->i1();
        ferr() <<  SemanticError(Format("Incomplete match against language \"%%\", falling back to \"plaintext\"") << language_->name(), text, offset) << nl;
        toki::Language *language = 0;
        toki::registry()->lookupLanguageByName("plaintext", &language);
        state_ = language->highlightingSyntax()->match(text_);
    }
}

Ref<MetaObject> CodeElement::produce()
{
    return CodeElement::create();
}

}} // namespace flux::doc
