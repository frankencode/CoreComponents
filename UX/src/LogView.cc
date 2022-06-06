/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LogView>
#include <cc/Text>
#include <cc/LineSource>
#include <cc/Format>

namespace cc {

struct LogView::State final: public ListView::State
{
    State()
    {
        font([this]{ return style().defaultFont(); });

        leadSpace([this]{ return margin()[1]; });
        tailSpace([this]{ return margin()[1]; });

        font.onChanged([this]{ reload(); });
    }

    void reload()
    {
        ListView::State::deplete();
        for (const String &line: lines_) {
            appendLine(line);
        }
    }

    void appendText(const String &text)
    {
        bool carrierAtEnd =
            carrier().height() <= height() ||
            carrier().pos()[1] <= height() - carrier().height() + sp(42);

        for (const String &line: LineSource{text}) {
            appendLine(line);
        }

        if (carrierAtEnd && height() < carrier().height()) {
            if (carrier().moving()) carrierStop();
            carrier().pos(Point{0, height() - carrier().height()});
        }
    }

    void appendLine(const String &line)
    {
        carrier().add(
            Text{line, font()}
            .margin([this]{ return Size{margin()[0], 0}; })
            .maxWidth([this]{ return width(); })
        );
    }

    void clearText()
    {
        ListView::State::deplete();
        lines_.deplete();
    }

    Property<Font> font;
    Property<Size> margin { sp(8), sp(8) };

    List<String> lines_;
};

LogView::LogView():
    ListView{onDemand<State>}
{}

LogView::LogView(double width, double height):
    ListView{new State}
{
    size(width, height);
}

LogView &LogView::associate(Out<LogView> self)
{
    return View::associate<LogView>(self);
}

Font LogView::font() const
{
    return me().font();
}

LogView &LogView::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

LogView &LogView::font(Definition<Font> &&f)
{
    me().font(move(f));
    return *this;
}

Size LogView::margin() const
{
    return me().margin();
}

LogView &LogView::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

LogView &LogView::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

LogView &LogView::appendText(const String &text)
{
    me().appendText(text);
    return *this;
}

void LogView::clearText()
{
    me().clearText();
}

List<String> LogView::textLines() const
{
    return me().lines_;
}

LogView::State &LogView::me()
{
    return Object::me.as<State>();
}

const LogView::State &LogView::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
