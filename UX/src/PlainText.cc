/*
 * Copyright (C) 2026 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PlainText>
#include <cc/GlyphRun>
#include <cc/FontMetrics>

namespace cc {

struct PlainText::State final: public View::State
{
    explicit State(const String &initialText = String{}, Font initialFont = Font{}):
       text{initialText},
       font{initialFont}
    {
        if (!initialFont) font([this]{ return style().defaultFixedFont(); });
        paper([this]{ return basePaper(); });

        glyphRun([this]{
            return GlyphRun{text, font()};
        });

        wrappedRun([this]{
            GlyphRun run = glyphRun();
            double maxLineWidth = maxWidth() - 2 * margin()[0];
            if (0 < maxLineWidth && maxLineWidth < run.size()[0]) {
                run = run.wrap(maxLineWidth);
            }
            return run;
        });

        size([this]{ return preferredSize(); });

        if (font().color())
            color(font().color());
        else
            color([this]{ return theme().primaryTextColor(); });

        metrics([this]{ return FontMetrics{font()}; });

        textPos([this]{
            return margin() + Point{0, metrics().ascender()};
        });

        paint([this]{
            if (getInk_ && getPaper_) {
                Painter{this}
                .setPen(color())
                .showGlyphRun(textPos().round(), wrappedRun(), getInk_, getPaper_);
            }
            else {
                Painter{this}
                .setPen(color())
                .showGlyphRun(textPos().round(), wrappedRun());
            }
        });
    }

    Size preferredSize() const override
    {
        return (0 < maxWidth() ? wrappedRun().size() : glyphRun().size()) + 2 * margin();
    }

    Size minSize() const override
    {
        return Size{0, wrappedRun().size()[1]} + 2 * margin();
    }

    Property<String> text;
    Property<Font> font;
    Property<Color> color;
    Property<Size> margin;
    Property<double> maxWidth;
    Property<FontMetrics> metrics;
    Property<Point> textPos;
    Property<GlyphRun> wrappedRun;
    Property<GlyphRun> glyphRun;
    GetColor getInk_;
    GetColor getPaper_;
};

PlainText::PlainText(Out<PlainText> self):
    View{onDemand<State>}
{
    View::associate<PlainText>(self);
}

PlainText::PlainText(const String &text, Out<PlainText> self):
    View{new State{text}}
{
    View::associate<PlainText>(self);
}

PlainText::PlainText(const String &text, Font font, Out<PlainText> self):
    View{new State{text, font}}
{
    View::associate<PlainText>(self);
}

PlainText &PlainText::associate(Out<PlainText> self)
{
    return View::associate<PlainText>(self);
}

String PlainText::text() const
{
    return me().text();
}

PlainText &PlainText::text(const String &newValue)
{
    me().text(newValue);
    return *this;
}

PlainText &PlainText::text(Definition<String> &&f)
{
    me().text(move(f));
    return *this;
}

Font PlainText::font() const
{
    return me().font();
}

PlainText &PlainText::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

PlainText &PlainText::font(Definition<Font> &&f)
{
    me().font(move(f));
    return *this;
}

Color PlainText::color() const
{
    return me().color();
}

PlainText &PlainText::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

PlainText &PlainText::color(Definition<Color> &&f)
{
    me().color(move(f));
    return *this;
}

Size PlainText::margin() const
{
    return me().margin();
}

PlainText &PlainText::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

PlainText &PlainText::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

double PlainText::maxWidth() const
{
    return me().maxWidth();
}

PlainText &PlainText::maxWidth(double newValue)
{
    me().maxWidth(newValue);
    return *this;
}

PlainText &PlainText::maxWidth(Definition<double> &&f)
{
    me().maxWidth(move(f));
    return *this;
}

PlainText &PlainText::getInk(GetColor &&f)
{
    me().getInk_ = std::move(f);
    return *this;
}

PlainText &PlainText::getPaper(GetColor &&f)
{
    me().getPaper_ = std::move(f);
    return *this;
}

PlainText::State &PlainText::me()
{
    return get<State>();
}

const PlainText::State &PlainText::me() const
{
    return get<State>();
}

} // namespace cc
