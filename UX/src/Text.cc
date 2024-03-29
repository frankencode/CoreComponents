/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TextState>

namespace cc {

Text::Text():
    View{onDemand<State>}
{}

Text::Text(const String &text, Font font):
    View{new State{text, font}}
{}

Text::Text(const String &text, Font font, bool html):
    View{new State{text, font, html}}
{}

Text::Text(State *newState):
    View{newState}
{}

Text::Text(CreateState onDemand):
    View{onDemand}
{}

Text &Text::associate(Out<Text> self)
{
    return View::associate<Text>(self);
}

String Text::text() const
{
    return me().text();
}

Text &Text::text(const String &newValue)
{
    me().text(newValue);
    return *this;
}

Text &Text::text(Definition<String> &&f)
{
    me().text(move(f));
    return *this;
}

Font Text::font() const
{
    return me().font();
}

Text &Text::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

Text &Text::font(Definition<Font> &&f)
{
    me().font(move(f));
    return *this;
}

Color Text::color() const
{
    return me().color();
}

Text &Text::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

Text &Text::color(Definition<Color> &&f)
{
    me().color(move(f));
    return *this;
}

Size Text::margin() const
{
    return me().margin();
}

Text &Text::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

Text &Text::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

double Text::maxWidth() const
{
    return me().maxWidth();
}

Text &Text::maxWidth(double newValue)
{
    me().maxWidth(newValue);
    return *this;
}

Text &Text::maxWidth(Definition<double> &&f)
{
    me().maxWidth(move(f));
    return *this;
}

TextAlign Text::textAlign() const
{
    return me().textAlign();
}

Text &Text::textAlign(TextAlign newValue)
{
    me().textAlign(newValue);
    return *this;
}

Text &Text::textAlign(Definition<TextAlign> &&f)
{
    me().textAlign(move(f));
    return *this;
}

Point Text::baselineStart() const
{
    return me().baselineStart();
}

Text &Text::baselineStart(Definition<Point> &&a)
{
    me().baselineStart(move(a));
    return *this;
}

FontMetrics Text::fontMetrics() const
{
    return me().metrics();
}

Text::State &Text::me()
{
    return get<State>();
}

const Text::State &Text::me() const
{
    return get<State>();
}

} // namespace cc
