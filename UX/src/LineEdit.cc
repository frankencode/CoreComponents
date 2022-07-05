/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineEdit>
#include <cc/TextInput>
#include <cc/Label>
#include <cc/Text>
#include <cc/Painter>

namespace cc {

struct LineEdit::State: public Control::State
{
    explicit State(const String &title = String{});
    State(const TextInput &input, const String &title);

    void settled() override;
    void polish() override;

    void setLeading(const View &newView);
    void setTrailing(const View &newView);

    void setAlertIcon();

    void editPlus();

    Control delegate() const override;

    Size preferredSize() const override;
    Size minSize() const override;
    Size maxSize() const override;

    Property<String> title;
    Property<String> text;
    Property<String> placeholder;
    Property<String> help;
    Property<String> error;
    Property<String> status;

    Property<View> leading;
    Property<View> trailing;

    Property<Color> polishPaper;
    Property<double> underlineThickness;
    Property<Color> underlineColor;
    Property<double> messageHeight;

    bool editPlus_ { false };
    bool firstTime_ { true };

    TextInput input_;
    Label titleLabel_, smallLabel_, bigLabel_;
    View underline_;
    Label placeholderOverlay_;

    Text helpView_;
    Text errorView_;
    Label statusView_;
};

LineEdit::State::State(const String &title):
    State{TextInput{}, title}
{}

LineEdit::State::State(const TextInput &input, const String &initialTitle):
    title{initialTitle},
    input_{input}
{
    polishPaper([this]{ return basePaper(); });
    paper([this]{ return theme().inputFieldFillColor(); });

    input_.pos(Point{dp(16), dp(24)}.ceil());
    input_.size([this]{
        return size() - Size{dp(28), dp(32) + messageHeight()}.ceil();
    });
    add(input_);

    text([this]{ return input_.text(); });

    size([this]{ return preferredSize(); });

    add(
        Label{}
        .associate(&smallLabel_)
        .text([this]{ return title(); })
        .font([this]{ return style().defaultSmallFont(); })
        .pos([]{ return Point{dp(16), dp(12)}; })
        .visible(false)
    );

    add(
        Label{}
        .associate(&bigLabel_)
        .text([this]{ return title(); })
        .font([this]{ return style().defaultFont(); })
        .pos([this]{
            return Point{leading() ? dp(56) : dp(16), (size()[1] - messageHeight()) / 2 - bigLabel_.size()[1] / 2};
        })
        .visible(false)
    );

    add(
        Label{}
        .associate(&titleLabel_)
        .text([this]{ return title(); })
        .color([this]{
            return (pressed() || input_.focus()) ? theme().focusTextColor() : theme().secondaryTextColor();
        })
        .font([this]{
            return (input_.focus() || input_.text().count() > 0) ?
                smallLabel_.font() :
                bigLabel_.font();
        })
        .pos([this]{
            return (input_.focus() || input_.text().count() > 0) ?
                smallLabel_.pos() :
                bigLabel_.pos();
        })
    );

    input_.add(
        Label{}
        .associate(&placeholderOverlay_)
        .text([this]{ return placeholder(); })
        .color([this]{ return theme().inactiveTextColor(); })
        .visible([this]{ return input_.focus() && placeholder().count() > 0 && text().count() == 0 && titleLabel_.pos() == smallLabel_.pos(); })
        .pos([this]{
            return Point{
                sp(1),
                input_.size()[1] / 2 - placeholderOverlay_.size()[1] / 2
            };
        })
    );

    {
        underlineThickness([this]{ return (hover() || pressed() || input_.focus()) && error().count() == 0 ? dp(2) : dp(1); });
        underlineColor([this]{
            if (error().count() > 0) return theme().alertColor();
            if (pressed()) return theme().pressedInputLineColor();
            if (input_.focus()) return theme().focusInputLineColor();
            if (hover()) return theme().hoverInputLineColor();
            return theme().inputLineColor();
        });

        add(
            View{}
            .associate(&underline_)
            .paper([this]{ return paper(); })
            .size([this]{
                return Size{size()[0], std::ceil(dp(2))};
            })
            .pos([this]{
                return Point{0, std::ceil(size()[1]) - underline_.size()[1] - messageHeight()};
            })
            .paint([this]{
                Painter p{underline_};
                p.rectangle(Point{0, underline_.height() - underlineThickness()}, underline_.size());
                p.setPen(underlineColor());
                p.fill();
            })
        );
    }

    paint([this]{
        const double w = size()[0];
        const double h = size()[1] - messageHeight();
        const double r = dp(6);

        Painter p{this};
        p.moveTo(Point{0, h});
        p.lineTo(Point{0, r});
        p.arc(Point{r, r}, r, rad(180), rad(270));
        p.lineTo(Point{w - r, 0});
        p.arc(Point{w - r, r}, r, rad(270), rad(360));
        p.lineTo(Point{w, h});
        p.setPen(paper());
        p.fill();
    });
}

void LineEdit::State::settled()
{
    Easing::Bezier curve{0.5, 0.0, 0.5, 1.0};
    const double duration = 0.1;
    titleLabel_
        .fontEasing(curve, duration)
        .posEasing(curve, duration);
}

void LineEdit::State::polish()
{
    clear(polishPaper());
}

void LineEdit::State::setLeading(const View &newView)
{
    if (leading()) {
        leading().visible(false);
        remove(leading());
    }
    leading(newView);
    leading().centerLeft([this]{ return Point{sp(16) + 1, sp(26)}; });
    leading().visible([this]{ return !focus() && text().count() == 0; });
    add(leading());
}

void LineEdit::State::setTrailing(const View &newView)
{
    if (trailing()) {
        trailing().visible(false);
        remove(trailing());
    }
    trailing(newView);
    trailing().bottomRight([this]{ return Point{width() - sp(12), input_.y() + input_.textPos().y() }; });
    add(trailing());
}

void LineEdit::State::setAlertIcon()
{
    setTrailing(
        Picture{Icon::AlertCircle}
        .color([this]{ return theme().alertColor(); })
        .visible([this]{ return error() != ""; })
    );
}

void LineEdit::State::editPlus()
{
    if (editPlus_) return;
    editPlus_ = true;

    add(
        View{}
        .bottomLeft([this]{ return Size{dp(16), height()}; })
        .add(
            Label{}
            .associate(&statusView_)
            .text([this]{ return status(); })
            .font([this]{ return style().defaultSmallFont(); })
            .baselineEnd([this]{ return Point{width() - dp(24), dp(16)}; })
            .color([this]{ return theme().secondaryTextColor(); })
            .paper([this]{ return basePaper(); })
            .visible([this]{ return status() != ""; })
        )
        .add(
            Text{}
            .associate(&helpView_)
            .text([this]{ return help(); })
            .font([this]{ return style().defaultSmallFont(); })
            .maxWidth([this]{ return width() - dp(28) - (statusView_.visible() ? statusView_.width() + dp(8) : 0); })
            .baselineStart([]{ return Point{0, dp(16)}; })
            .color([this]{ return theme().secondaryTextColor(); })
            .paper([this]{ return basePaper(); })
            .visible([this]{ return help() != "" && error() == ""; })
        )
        .add(
            Text{}
            .associate(&errorView_)
            .text([this]{ return error(); })
            .font([this]{ return style().defaultSmallFont(); })
            .maxWidth([this]{ return width() - dp(28) - (statusView_.visible() ? statusView_.width() + dp(8) : 0); })
            .baselineStart([]{ return Point{0, dp(16)}; })
            .color([this]{ return theme().alertColor(); })
            .paper([this]{ return basePaper(); })
            .visible([this]{ return error() != ""; })
        )
        .size([this]{ return Size{width() - dp(28), messageHeight()}; })
    );

    messageHeight([this]{
        if (help().count() == 0 && error().count() == 0 && status().count() == 0) return 0.;
        double hh = helpView_.height() + dp(16) - helpView_.fontMetrics().ascender();
        double he = errorView_.height() + dp(16) - errorView_.fontMetrics().ascender();
        double hs = statusView_.height() + dp(16) - statusView_.fontMetrics().ascender();
        double h = hh;
        if (h < he) h = he;
        if (h < hs) h = hs;
        return h;
    });

    text.onChanged([this]{ error = String{}; });
}

Control LineEdit::State::delegate() const
{
    return input_;
}

Size LineEdit::State::preferredSize() const
{
    Size s = input_.preferredSize() + Size{dp(28), dp(32) + messageHeight()}.ceil();
    s[0] = expandableWidth(s[0]);
    return s;
}

Size LineEdit::State::minSize() const
{
    return input_.minSize() + Size{dp(28), dp(32)}.ceil();
}

Size LineEdit::State::maxSize() const
{
    return input_.maxSize() + Size{dp(28), dp(32)}.ceil();
}

LineEdit::LineEdit():
    Control{onDemand<State>}
{}

LineEdit::LineEdit(const String &label):
    Control{new State{label}}
{}

LineEdit::LineEdit(const TextInput &input, const String &title):
    Control{new State{input, title}}
{}

LineEdit &LineEdit::associate(Out<LineEdit> self)
{
    return View::associate<LineEdit>(self);
}

String LineEdit::title() const
{
    return me().title();
}

LineEdit &LineEdit::title(const String &newValue)
{
    me().title(newValue);
    return *this;
}

String LineEdit::text() const
{
    return me().text();
}

LineEdit &LineEdit::text(const String &newValue)
{
    me().input_.text(newValue).selectAll(); // FIXME move selectAll() to TextInput
    return *this;
}

String LineEdit::placeholder() const
{
    return me().placeholder();
}

LineEdit &LineEdit::placeholder(const String &newValue)
{
    me().placeholder(newValue);
    return *this;
}

String LineEdit::help() const
{
    return me().help();
}

LineEdit &LineEdit::help(const String &newValue)
{
    me().editPlus();
    me().help(newValue);
    return *this;
}

LineEdit &LineEdit::help(Definition<String> &&f)
{
    me().editPlus();
    me().help(move(f));
    return *this;
}

String LineEdit::error() const
{
    return me().error();
}

LineEdit &LineEdit::error(const String &newValue)
{
    me().editPlus();
    me().error(newValue);
    return *this;
}

LineEdit &LineEdit::error(Definition<String> &&f)
{
    me().editPlus();
    me().error(move(f));
    return *this;
}

String LineEdit::status() const
{
    return me().status();
}

LineEdit &LineEdit::status(const String &newValue)
{
    me().editPlus();
    me().status(newValue);
    return *this;
}

LineEdit &LineEdit::status(Definition<String> &&f)
{
    me().editPlus();
    me().status(move(f));
    return *this;
}

LineEdit &LineEdit::icon(const Picture &picture)
{
    me().setLeading(picture);
    return *this;
}

LineEdit &LineEdit::alertIcon()
{
    me().setAlertIcon();
    return *this;
}

LineEdit &LineEdit::leading(const View &view)
{
    me().setLeading(view);
    return *this;
}

LineEdit &LineEdit::trailing(const View &view)
{
    me().setTrailing(view);
    return *this;
}

LineEdit &LineEdit::onAccepted(Fun<void()> &&f)
{
    me().input_.onAccepted(move(f));
    return *this;
}

LineEdit &LineEdit::onRejected(Fun<void()> &&f)
{
    me().input_.onRejected(move(f));
    return *this;
}

LineEdit &LineEdit::accept(Fun<bool()> &&f)
{
    me().input_.accept(move(f));
    return *this;
}

LineEdit &LineEdit::filter(Fun<bool(Range range, InOut<String>)> &&f)
{
    me().input_.filter(move(f));
    return *this;
}

TextInput LineEdit::input() const
{
    return me().input_;
}

LineEdit::State &LineEdit::me()
{
    return View::me().as<State>();
}

const LineEdit::State &LineEdit::me() const
{
    return View::me().as<State>();
}

} // namespace cc
