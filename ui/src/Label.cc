/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Label>
#include <cc/Painter>
#include <cc/GlyphRun>

namespace cc {

struct Label::State: public View::State
{
    State(const String &initText = String{}, const Font &initFont = String{}):
        text{initText},
        font{initFont}
    {
        if (font()) {
            if (font().paper())
                paper(font().paper());
        }
        else
            font([this]{ return style().defaultFont(); });

        if (!paper()) paper([this]{ return basePaper(); });

        glyphRun([this]{
            return GlyphRun{text(), font()};
        });

        elidedRun([this]{
            if (0 < maxWidth()) {
                double maxLineWidth = maxWidth() - 2 * margin()[0];
                if (maxLineWidth < glyphRun().size()[0]) {
                    return glyphRun().elide(maxLineWidth);
                }
            }
            return glyphRun();
        });

        size([this]{ return preferredSize(); });

        if (font().color())
            color(font().color());
        else
            color([this]{ return theme().primaryTextColor(); });

        paint([this]{
            Painter p{this};
            p.setPen(color());
            p.showGlyphRun(textPos().round(), elidedRun());
        });
    }

    Size preferredSize() const override
    {
        return preferredSize(GlyphRun{text, font}, margin);
    }

    Size preferredSize(const GlyphRun &glyphRun, Size margin) const
    {
        Size size = 2 * margin +
            Size{
                glyphRun.size()[0],
                glyphRun.maxAscender() - glyphRun.minDescender()
            };

        if (0 < maxWidth && maxWidth < size[0]) size[0] = maxWidth;

        return size.ceil();
    }

    Size minSize() const override
    {
        return preferredSize();
    }

    Size maxSize() const override
    {
        return preferredSize();
    }

    Point textPos(double relativeHeight = 0) const
    {
        GlyphRun run = elidedRun();

        return
            0.5 * (
                size() -
                Size {
                    run.size()[0],
                    run.maxAscender() - run.minDescender()
                }
            ) +
            Size {
                0,
                run.maxAscender() * (1 - relativeHeight)
            };
    }

    void baselineStart(Definition<Point> &&a)
    {
        pos([this, a]{ return a() - textPos(); });
    }

    void baselineEnd(Definition<Point> &&a)
    {
        pos([this, a]{ return a() - textPos() - Point{width(), 0}; });
    }

    void textCenterLeft(Definition<Point> &&a)
    {
        pos([this, a]{ return a() - textPos(0.5); });
    }

    Property<String> text; ///< %Text displayed by the label
    Property<Font> font; ///< %Font used
    Property<Color> color; ///< %Foreground color
    Property<Size> margin; ///< %Outside margin around the text
    Property<double> maxWidth;
    Property<GlyphRun> glyphRun;
    Property<GlyphRun> elidedRun;
};

Label::Label():
    View{onDemand<State>}
{}

Label::Label(Out<Label> self):
    View{new State}
{
    self = weak<Label>();
}

Label::Label(const String &text, Out<Label> self):
    View{new State{text}}
{
    self = weak<Label>();
}

Label::Label(const String &text, const Font &font, Out<Label> self):
    View{new State{text, font}}
{
    self = weak<Label>();
}

String Label::text() const
{
    return me().text();
}

Label &Label::text(const String &newValue)
{
    me().text(newValue);
    return *this;
}

Label &Label::text(Definition<String> &&f)
{
    me().text(std::move(f));
    return *this;
}

Font Label::font() const
{
    return me().font();
}

Label &Label::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

Label &Label::font(Definition<Font> &&f)
{
    me().font(std::move(f));
    return *this;
}

Label &Label::fontEasing(const EasingCurve &easing, double duration)
{
    Easing{me().font, easing, duration};
    return *this;
}

Color Label::color() const
{
    return me().color();
}

Label &Label::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

Label &Label::color(Definition<Color> &&f)
{
    me().color(std::move(f));
    return *this;
}

Label &Label::colorEasing(const EasingCurve &easing, double duration)
{
    Easing{me().font, easing, duration};
    return *this;
}

Size Label::margin() const
{
    return me().margin();
}

Label &Label::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

Label &Label::margin(Definition<Size> &&f)
{
    me().margin(std::move(f));
    return *this;
}

double Label::maxWidth() const
{
    return me().maxWidth();
}

Label &Label::maxWidth(double newValue)
{
    me().maxWidth(newValue);
    return *this;
}

Label &Label::maxWidth(Definition<double> &&f)
{
    me().maxWidth(std::move(f));
    return *this;
}

Point Label::textPos() const
{
    return me().textPos();
}

Point Label::textCenterLeft() const
{
    return me().textPos(0.5);
}

Label &Label::baselineStart(Definition<Point> &&a)
{
    me().baselineStart(std::move(a));
    return *this;
}

Label &Label::baselineEnd(Definition<Point> &&a)
{
    me().baselineEnd(std::move(a));
    return *this;
}

Label &Label::textCenterLeft(Definition<Point> &&a)
{
    me().textCenterLeft(std::move(a));
    return *this;
}

FontMetrics Label::fontMetrics() const
{
    return FontMetrics{font()};
}

Label::State &Label::me()
{
    return View::me().as<State>();
}

const Label::State &Label::me() const
{
    return View::me().as<State>();
}

} // namespace cc
