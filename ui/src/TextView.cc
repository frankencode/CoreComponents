/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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

        text.onChanged([this]{ reload(); });
        font.onChanged([this]{ reload(); });

        if (text() != "") reload();
    }

    void reload()
    {
        ListView::State::deplete();
        for (const String &line: LineSource{text()}) {
            pane().add(
                Text{line}
                .maxWidth([this]{ return width(); })
            );
        }
    }

    Property<String> text;
    Property<Font> font;
};

TextView::TextView():
    ListView{onDemand<State>}
{}

TextView::TextView(Out<TextView> self):
    ListView{new State}
{
    self = weak<TextView>();
}

TextView::TextView(double width, double height):
    ListView{new State}
{
    size(width, height);
}

TextView::TextView(const String &text, Out<TextView> self):
    ListView{new State{text}}
{
    self = weak<TextView>();
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
    me().text(std::move(f));
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
    me().font(std::move(f));
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
