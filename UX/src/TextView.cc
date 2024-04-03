/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TextView>
#include <cc/Text>
#include <cc/LineSource>
#include <cc/Format>

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

TextView::TextView():
    ListView{onDemand<State>}
{}

TextView::TextView(double width, double height):
    ListView{new State}
{
    size(width, height);
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

TextView::State &TextView::me()
{
    return Object::me.as<State>();
}

const TextView::State &TextView::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
