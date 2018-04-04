/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/StylePlugin>
#include <cc/ui/PlatformPlugin>
#include <cc/ui/TextRun>

namespace cc {
namespace ui {

Ref<TextRun> TextRun::create()
{
    return PlatformPlugin::instance()->createTypeSetter()->createTextRun();
}

Ref<TextRun> TextRun::create(String text, const TextStyle *style)
{
    if (!style) style = StylePlugin::instance()->defaultTextStyle();

    Ref<TextRun> textRun = TextRun::create();
    if (text->countCharsIn("<>&") > 0)
        textRun->appendHtml(text, style);
    else
        textRun->append(text, style);

    return textRun;
}

void TextRun::append(const TextRun *textRun)
{
    for (const GlyphRun *glyphRun: textRun->getAllGlyphRuns())
        append(glyphRun->text(), glyphRun->style());
}

void TextRun::appendHtml(String text, const TextStyle *style)
{
    struct StyleNode: public Object {
        StyleNode(const TextStyle *style, String tagName = String{}, const StyleNode *parent = nullptr):
            tagName_(tagName),
            style_(style->copy()),
            parent_(parent)
        {}
        String tagName_;
        Ref<const TextStyle> style_;
        Ref<const StyleNode> parent_;
    };

    Ref<const StyleNode> styleHead = new StyleNode(style);
    int i = 0;
    int i0 = 0;
    while (text->findNext('<', &i)) {
        int j = i + 1;
        if (!text->findNext('>', &j)) break;
        if (i0 < i) append(replaceEntities(text->copy(i0, i)), styleHead->style_);
        String tagName = text->select(i + 1, j);
        i = i0 = j + 1;
        if (tagName->beginsWith('/')) {
            tagName = tagName->select(1, tagName->count());
            if (styleHead->tagName_ != tagName) break;
            styleHead = styleHead->parent_;
        }
        else if (tagName == "u") {
            styleHead = new StyleNode(
                TextStyle::create(
                    styleHead->style_->font(),
                    styleHead->style_->decoration() | Decoration::Underline,
                    styleHead->style_->color()
                ),
                tagName,
                styleHead
            );
        }
        else if (tagName == "b") {
            styleHead = new StyleNode(
                TextStyle::create(
                    ScaledFont::select(
                        styleHead->style_->font()->family(),
                        styleHead->style_->font()->size(),
                        Weight::Bold,
                        styleHead->style_->font()->slant(),
                        styleHead->style_->font()->stretch()
                    ),
                    styleHead->style_->decoration(),
                    styleHead->style_->color()
                ),
                tagName,
                styleHead
            );
        }
        else if (tagName == "i") {
            styleHead = new StyleNode(
                TextStyle::create(
                    ScaledFont::select(
                        styleHead->style_->font()->family(),
                        styleHead->style_->font()->size(),
                        styleHead->style_->font()->weight(),
                        Slant::Italic,
                        styleHead->style_->font()->stretch()
                    ),
                    styleHead->style_->decoration(),
                    styleHead->style_->color()
                ),
                tagName,
                styleHead
            );
        }
        else if (tagName == "strike") {
            styleHead = new StyleNode(
                TextStyle::create(
                    styleHead->style_->font(),
                    styleHead->style_->decoration() | Decoration::StrikeOut,
                    styleHead->style_->color()
                ),
                tagName,
                styleHead
            );
        }
    }

    if (i0 < i) append(replaceEntities(text->copy(i0, i)), styleHead->style_);
}

String TextRun::replaceEntities(String text)
{
    if (text->contains('&')) {
        mutate(text)->replaceInsitu("&gt;", ">");
        mutate(text)->replaceInsitu("&lt;", "<");
        mutate(text)->replaceInsitu("&amp;", "&");
    }
    return text;
}

}} // namespace cc::ui
