/*
 * Copyright (C) 2022-2026 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TextView>
#include <cc/TextState>
#include <cc/LineSource>

namespace cc {

struct TextView::State final: public ListView::State
{
    State(const String &initialText = ""):
        text{initialText}
    {
        font([this]{ return style().defaultFont(); });

        margin([this]{
            double m = style().flickableIndicatorThickness();
            return Size{m, m};
        });

        leadSpace([this]{ return margin()[1]; });
        tailSpace([this]{ return margin()[1]; });

        text.onChanged([this]{ reload(); });
        font.onChanged([this]{ reload(); });

        if (text() != "") reload();
    }

    void reload()
    {
        ListView::State::deplete();
        for (const String &line: LineSource{text()}) {
            carrier().add(
                Text{line, font()}
                .margin([this]{ return Size{margin()[0], 0}; })
                .maxWidth([this]{ return width(); })
            );
        }
    }

    Property<String> text;
    Property<Font> font;
    Property<Size> margin;
};

TextView::TextView(Out<TextView> self):
    ListView{onDemand<State>}
{
    View::associate<TextView>(self);
}

TextView::TextView(const Size &size, Out<TextView> self):
    ListView{new State}
{
    View::size(size);
    View::associate<TextView>(self);
}

TextView::TextView(const String &text):
    ListView{new State{text}}
{}

TextView &TextView::associate(Out<TextView> self)
{
    return View::associate<TextView>(self);
}

String TextView::text() const
{
    return me().text();
}

TextView &TextView::text(const String &newValue)
{
    me().text(newValue);
    return *this;
}

TextView &TextView::text(Definition<String> &&f)
{
    me().text(move(f));
    return *this;
}

Font TextView::font() const
{
    return me().font();
}

TextView &TextView::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

TextView &TextView::font(Definition<Font> &&f)
{
    me().font(move(f));
    return *this;
}

Size TextView::margin() const
{
    return me().margin();
}

TextView &TextView::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

TextView &TextView::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

Text TextView::lineAt(long index) const
{
    return itemAt(index).as<Text>();
}

TextView::State &TextView::me()
{
    return Object::me.as<State>();
}

const TextView::State &TextView::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
