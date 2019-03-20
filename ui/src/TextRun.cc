/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

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
    textRun->append(text, font);
    return textRun;
}

Ref<TextRun> TextRun::createHtml(const String &text, const Font &font)
{
    Ref<TextRun> textRun = TextRun::create();
    textRun->appendHtml(text, font);
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
        if (tagName->startsWith('/')) {
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
    Ref<TextCursor> cursor = getTextCursor();
    Ref<TextCursor> nearestCursor = cursor->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    do {
        Point pos = 0.5 * (cursor->posA() + cursor->posB());
        const double distance = absPow2(pos - pointerPos);

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor->assign(cursor);
        }
    } while (cursor->step(1));

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestTextCursorBelow(const TextCursor *cursor) const
{
    Ref<TextCursor> lineStart = cursor->copy();
    do {
        if (!lineStart->step(1)) return cursor->copy();
    } while (lineStart->posA()[1] == cursor->posA()[1]);

    Ref<TextCursor> nearestCursor = lineStart->copy();
    Ref<TextCursor> candidate = lineStart->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate->posA()[1] == lineStart->posA()[1])
    {
        Point pos = 0.5 * (candidate->posA() + candidate->posB());
        const double distance = absPow2(cursor->posB() - pos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor->assign(candidate);
        }
        if (!candidate->step(1)) break;
    }

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestTextCursorAbove(const TextCursor *cursor) const
{
    Ref<TextCursor> lineStart = cursor->copy();
    do {
        if (!lineStart->step(-1)) return cursor->copy();
    } while (lineStart->posA()[1] == cursor->posA()[1]);

    Ref<TextCursor> nearestCursor = lineStart->copy();
    Ref<TextCursor> candidate = lineStart->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate->posA()[1] == lineStart->posA()[1])
    {
        Point pos = 0.5 * (candidate->posA() + candidate->posB());
        const double distance = absPow2(cursor->posB() - pos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor->assign(candidate);
        }
        if (!candidate->step(-1)) break;
    }

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestLineStart(Point pointerPos) const
{
    Ref<TextCursor> candidate = getTextCursor();
    Ref<TextCursor> nearestCursor = candidate->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    do {
        Point pos = 0.5 * (candidate->posA() + candidate->posB());
        const double distance = absPow2(pos - pointerPos);

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor->assign(candidate);
        }
    } while (candidate->lineStep(1));

    return nearestCursor;
}

Ref<TextCursor> TextRun::getNearestTextCursorInLine(Point pointerPos) const
{
    Ref<TextCursor> lineStart = getNearestLineStart(pointerPos);
    Ref<TextCursor> nearestCursor = lineStart->copy();
    Ref<TextCursor> candidate = lineStart->copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate->posA()[1] == lineStart->posA()[1])
    {
        Point pos = 0.5 * (candidate->posA() + candidate->posB());
        const double distance = absPow2(pos - pointerPos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor->assign(candidate);
        }
        if (!candidate->step(1)) break;
    }

    return nearestCursor;
}

}} // namespace cc::ui
