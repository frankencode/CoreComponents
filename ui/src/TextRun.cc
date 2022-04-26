/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TextRun>
#include <cc/FontManager>
#include <cc/Queue>
#include <limits>

namespace cc {

TextRun::TextRun()
{
    *this = FontManager{}.createTextRun();
}

TextRun::TextRun(const String &text, const Font &font):
    TextRun{}
{
    me().append(text, font);
}

TextRun TextRun::fromHtml(const String &text, const Font &font)
{
    TextRun textRun = FontManager{}.createTextRun();
    textRun.appendHtml(text, font);
    return textRun;
}

TextRun &TextRun::appendHtml(const String &text, const Font &font)
{
    struct StylePlugin {
        StylePlugin(const Font &font, const String &tagName = String{}):
            tagName_{tagName},
            font_{font}
        {}
        String tagName_;
        Font font_;
    };

    Queue<StylePlugin> styling;
    styling.emplaceBack(font);

    long i = 0;
    long i0 = 0;
    while (text.find('<', &i)) {
        long j = i + 1;
        if (!text.find('>', &j)) break;
        if (i0 < i) append(replaceEntities(text.copy(i0, i)), styling.last().font_);
        String tagName = text.copy(i + 1, j);
        i = i0 = j + 1;
        if (tagName.startsWith('/')) {
            tagName = tagName.copy(1, tagName.count());
            if (styling.last().tagName_ != tagName || styling.count() == 1) break;
            styling.popBack();
        }
        else if (tagName == "u") {
            Font font = styling.last().font_;
            font.decoration(font.decoration() | Decoration::Underline);
            styling.emplaceBack(font, tagName);
        }
        else if (tagName == "b") {
            Font font = styling.last().font_;
            font.weight(Weight::Bold);
            styling.emplaceBack(font, tagName);
        }
        else if (tagName == "i") {
            Font font = styling.last().font_;
            font.slant(Slant::Italic);
            styling.emplaceBack(font, tagName);
        }
        else if (tagName == "strike") {
            Font font = styling.last().font_;
            font.decoration(Decoration::StrikeOut);
            styling.emplaceBack(font, tagName);
        }
    }

    if (i0 < i) append(replaceEntities(text.copy(i0, i)), styling.last().font_);

    return *this;
}

TextCursor TextRun::getNearestTextCursor(Point pointerPos) const
{
    TextCursor cursor = getTextCursor();
    TextCursor nearestCursor = cursor.copy();
    double nearestDistance = std::numeric_limits<double>::max();

    do {
        Point pos = 0.5 * (cursor.posA() + cursor.posB());
        const double distance = (pos - pointerPos).absPow2();

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor.assign(cursor);
        }
    } while (cursor.step(1));

    return nearestCursor;
}

TextCursor TextRun::getNearestTextCursorBelow(const TextCursor &cursor) const
{
    TextCursor lineStart = cursor.copy();
    do {
        if (!lineStart.step(1)) return cursor.copy();
    } while (lineStart.posA()[1] == cursor.posA()[1]);

    TextCursor nearestCursor = lineStart.copy();
    TextCursor candidate = lineStart.copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate.posA()[1] == lineStart.posA()[1])
    {
        Point pos = 0.5 * (candidate.posA() + candidate.posB());
        const double distance = (cursor.posB() - pos).absPow2();
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor.assign(candidate);
        }
        if (!candidate.step(1)) break;
    }

    return nearestCursor;
}

TextCursor TextRun::getNearestTextCursorAbove(const TextCursor &cursor) const
{
    TextCursor lineStart = cursor.copy();
    do {
        if (!lineStart.step(-1)) return cursor.copy();
    } while (lineStart.posA()[1] == cursor.posA()[1]);

    TextCursor nearestCursor = lineStart.copy();
    TextCursor candidate = lineStart.copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate.posA()[1] == lineStart.posA()[1])
    {
        Point pos = 0.5 * (candidate.posA() + candidate.posB());
        const double distance = (cursor.posB() - pos).absPow2();
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor.assign(candidate);
        }
        if (!candidate.step(-1)) break;
    }

    return nearestCursor;
}

TextCursor TextRun::getNearestLineStart(Point pointerPos) const
{
    TextCursor candidate = getTextCursor();
    TextCursor nearestCursor = candidate.copy();
    double nearestDistance = std::numeric_limits<double>::max();

    do {
        Point pos = 0.5 * (candidate.posA() + candidate.posB());
        const double distance = (pos - pointerPos).absPow2();

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor.assign(candidate);
        }
    } while (candidate.lineStep(1));

    return nearestCursor;
}

TextCursor TextRun::getNearestTextCursorInLine(Point pointerPos) const
{
    TextCursor lineStart = getNearestLineStart(pointerPos);
    TextCursor nearestCursor = lineStart.copy();
    TextCursor candidate = lineStart.copy();
    double nearestDistance = std::numeric_limits<double>::max();

    while (candidate.posA()[1] == lineStart.posA()[1])
    {
        Point pos = 0.5 * (candidate.posA() + candidate.posB());
        const double distance = (pos - pointerPos).absPow2();
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestCursor.assign(candidate);
        }
        if (!candidate.step(1)) break;
    }

    return nearestCursor;
}

String TextRun::replaceEntities(String text)
{
    if (text.find('&')) {
        text.replace("&gt;", ">");
        text.replace("&lt;", "<");
        text.replace("&amp;", "&");
    }
    return text;
}

} //
