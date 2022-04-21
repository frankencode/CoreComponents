/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Text>
#include <cc/ui/TextRun>
#include <cc/ui/FontMetrics>

namespace cc::ui {

struct Text::State: public View::State
{
    State(const String &initialText = String{}, Font initialFont = Font{}):
       text{initialText},
       font{initialFont}
    {
        if (!initialFont) font([this]{ return style().defaultFont(); });
        paper([this]{ return basePaper(); });

        textRun([this]{
            return TextRun{text, font()};
            // return TextRun::fromHtml(text(), font());
        });

        wrappedRun([this]{
            TextRun run = textRun();
            #if 0
            double maxLineWidth = size()[0];
            if (0 < maxWidth() && maxWidth() < maxLineWidth) maxLineWidth = maxWidth();
            maxLineWidth -=  2 * margin()[0];
            #endif
            double maxLineWidth = maxWidth() - 2 * margin()[0];
            if (0 < maxLineWidth && maxLineWidth < run.size()[0]) {
                run = run.wrap(maxLineWidth, textAlign());
            }
            return run;
        });

        size([this]{ return preferredSize(); });

        if (font().color())
            color(font().color());
        else
            color([this]{ return theme().primaryTextColor(); });

        textPos([this]{
            if (textAlign() == TextAlign::Left || textAlign() == TextAlign::Justify)
                return margin() + Point{0, metrics().ascender()};
            else if (textAlign() == TextAlign::Right)
                return Point{width() - margin()[0], margin()[1]} - wrappedRun().size()[0] + Point{0, metrics().ascender()};
            else
                return (size() - wrappedRun().size()) / 2 + Point{0, metrics().ascender()};
        });

        metrics([this]{ return FontMetrics{font()}; });

        paint([this]{
            Painter p{this};
            p.setPen(color());
            p.showTextRun(textPos().round(), wrappedRun());
        });
    }

    Size preferredSize() const override
    {
        return (0 < maxWidth() ? wrappedRun().size() : textRun().size()) + 2 * margin();
    }

    Size minSize() const override
    {
        return Size{0, wrappedRun().size()[1]} + 2 * margin();
    }

    void baselineStart(Definition<Point> &&a)
    {
        pos([this, a]{ return a() - textPos(); });
    }

    Property<String> text;
    Property<Font> font;
    Property<Color> color;
    Property<Size> margin;
    Property<double> maxWidth;
    Property<TextAlign> textAlign { TextAlign::Left };
    Property<Point> textPos;
    Property<FontMetrics> metrics;
    Property<TextRun> textRun;
    Property<TextRun> wrappedRun;
};

Text::Text():
    View{onDemand<State>}
{}

Text::Text(Out<Text> self):
    View{new State}
{
    self = *this;
}

Text::Text(const String &text, Out<Text> self):
    View{new State{text}}
{
    self = *this;
}

Text::Text(const String &text, Font font, Out<Text> self):
    View{new State{text, font}}
{
    self = *this;
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
    me().text(std::move(f));
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
    me().font(std::move(f));
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
    me().color(std::move(f));
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
    me().margin(std::move(f));
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
    me().maxWidth(std::move(f));
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
    me().textAlign(std::move(f));
    return *this;
}

Text &Text::baselineStart(Definition<Point> &&a)
{
    me().baselineStart(std::move(a));
    return *this;
}

FontMetrics Text::fontMetrics() const
{
    return me().metrics();
}

Text::State &Text::me()
{
    return Object::me.as<State>();
}

const Text::State &Text::me() const
{
    return Object::me.as<State>();
}

}
