/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <limits>
#include <cc/ui/StylePlugin>
#include <cc/ui/FontManager>
#include <cc/ui/TextRun>

namespace cc {
namespace ui {

Ref<TextRun> TextRun::create()
{
    return FontManager::instance()->createTextRun();
}

Ref<TextRun> TextRun::create(const String &text, const Font &font)
{
    Ref<TextRun> textRun = TextRun::create();
    if (text->countCharsIn("<>&") > 0)
        textRun->appendHtml(text, font);
    else
        textRun->append(text, font);

    return textRun;
}

void TextRun::append(const TextRun *textRun)
{
    for (const GlyphRun *glyphRun: textRun->getAllGlyphRuns())
        append(glyphRun->text(), glyphRun->font());
}

void TextRun::appendHtml(const String &text, const Font &font)
{
    struct StyleNode: public Object {
        StyleNode(const Font &font, String tagName = String{}, const StyleNode *parent = nullptr):
            tagName_(tagName),
            font_(font),
            parent_(parent)
        {}
        String tagName_;
        Font font_;
        Ref<const StyleNode> parent_;
    };

    Ref<const StyleNode> styleHead = new StyleNode(font);
    int i = 0;
    int i0 = 0;
    while (text->findNext('<', &i)) {
        int j = i + 1;
        if (!text->findNext('>', &j)) break;
        if (i0 < i) append(replaceEntities(text->copy(i0, i)), styleHead->font_);
        String tagName = text->select(i + 1, j);
        i = i0 = j + 1;
        if (tagName->beginsWith('/')) {
            tagName = tagName->select(1, tagName->count());
            if (styleHead->tagName_ != tagName) break;
            styleHead = styleHead->parent_;
        }
        else if (tagName == "u") {
            Font font = styleHead->font_;
            font->setDecoration(font->decoration() | Decoration::Underline);
            styleHead = new StyleNode(font, tagName, styleHead);
        }
        else if (tagName == "b") {
            Font font = styleHead->font_;
            font->setWeight(Weight::Bold);
            styleHead = new StyleNode(font, tagName, styleHead);
        }
        else if (tagName == "i") {
            Font font = styleHead->font_;
            font->setSlant(Slant::Italic);
            styleHead = new StyleNode(font, tagName, styleHead);
        }
        else if (tagName == "strike") {
            Font font = styleHead->font_;
            font->setDecoration(Decoration::StrikeOut);
            styleHead = new StyleNode(font, tagName, styleHead);
        }
    }

    if (i0 < i) append(replaceEntities(text->copy(i0, i)), styleHead->font_);
}

String TextRun::replaceEntities(const String &text)
{
    if (text->contains('&')) {
        mutate(text)->replaceInsitu("&gt;", ">");
        mutate(text)->replaceInsitu("&lt;", "<");
        mutate(text)->replaceInsitu("&amp;", "&");
    }
    return text;
}

double TextRun::lineHeight() const
{
    return size()[1] / lineCount();
}

Ref<TextCursor> TextRun::getNearestTextCursor(Point pointerPos) const
{
    Ref<TextCursor> nearestCursor;
    double nearestDistance = std::numeric_limits<double>::max();

    Ref<TextCursor> cursor = getTextCursor();
    do {
        const double distanceA = absPow2((cursor->posA() - pointerPos));
        const double distanceB = absPow2((cursor->posB() - pointerPos));

        if (distanceA < nearestDistance || distanceB < nearestDistance) {
            nearestDistance = (distanceA < distanceB) ? distanceA : distanceB;
            nearestCursor = cursor->copy();
        }
    } while (cursor->step(1));

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestTextCursorBelow(const TextCursor *cursor) const
{
    Ref<TextCursor> nearestCursor = cursor->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    Point targetPos = cursor->advance() + 0.5 * Step { 0, lineHeight() };
    Ref<TextCursor> candidate = cursor->copy();
    do {
        if (candidate->advance()[1] <= cursor->advance()[1]) continue;

        const double distanceA = absPow2((candidate->posA() - targetPos));
        const double distanceB = absPow2((candidate->posB() - targetPos));

        if (distanceA < nearestDistance || distanceB < nearestDistance) {
            nearestDistance = (distanceA < distanceB) ? distanceA : distanceB;
            nearestCursor = candidate->copy();
        }
    } while (candidate->step(1));

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestTextCursorAbove(const TextCursor *cursor) const
{
    Ref<TextCursor> nearestCursor = cursor->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    Point targetPos = cursor->advance() - 0.5 * Step { 0, lineHeight() };
    Ref<TextCursor> candidate = cursor->copy();
    do {
        if (candidate->advance()[1] >= cursor->advance()[1]) continue;

        const double distanceA = absPow2((candidate->posA() - targetPos));
        const double distanceB = absPow2((candidate->posB() - targetPos));

        if (distanceA < nearestDistance || distanceB < nearestDistance) {
            nearestDistance = (distanceA < distanceB) ? distanceA : distanceB;
            nearestCursor = candidate->copy();
        }
    } while (candidate->step(-1));

    return nearestCursor;
}

}} // namespace cc::ui
