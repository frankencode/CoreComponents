/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextField>
#include <cc/ui/TextInput>
#include <cc/ui/Label>
#include <cc/ui/Text>
#include <cc/ui/Icon>
#include <cc/ui/Painter>

namespace cc::ui {

struct TextField::State: public Control::State
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

TextField::State::State(const String &title):
    State{TextInput{}, title}
{}

TextField::State::State(const TextInput &input, const String &initialTitle):
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
        Label{&smallLabel_}
        .text([this]{ return title(); })
        .font([this]{ return style().defaultSmallFont(); })
        .pos([]{ return Point{dp(16), dp(12)}; })
        .visible(false)
    );

    add(
        Label{&bigLabel_}
        .text([this]{ return title(); })
        .font([this]{ return style().defaultFont(); })
        .pos([this]{
            return Point{leading() ? dp(56) : dp(16), (size()[1] - messageHeight()) / 2 - bigLabel_.size()[1] / 2};
        })
        .visible(false)
    );

    add(
        Label{&titleLabel_}
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
        Label{&placeholderOverlay_}
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
            View{&underline_}
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
        p.arc(Point{r, r}, r, degree(180), degree(270));
        p.lineTo(Point{w - r, 0});
        p.arc(Point{w - r, r}, r, degree(270), degree(360));
        p.lineTo(Point{w, h});
        p.setPen(paper());
        p.fill();
    });
}

void TextField::State::settled()
{
    Easing::Bezier curve{0.5, 0.0, 0.5, 1.0};
    const double duration = 0.1;
    titleLabel_
        .fontEasing(curve, duration)
        .posEasing(curve, duration);
}

void TextField::State::polish()
{
    clear(polishPaper());
}

void TextField::State::setLeading(const View &newView)
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

void TextField::State::setTrailing(const View &newView)
{
    if (trailing()) {
        trailing().visible(false);
        remove(trailing());
    }
    trailing(newView);
    trailing().bottomRight([this]{ return Point{width() - sp(12), input_.y() + input_.textPos().y() }; });
    add(trailing());
}

void TextField::State::setAlertIcon()
{
    setTrailing(
        Icon{Ideographic::AlertCircle}
        .color([this]{ return theme().alertColor(); })
        .visible([this]{ return error() != ""; })
    );
}

void TextField::State::editPlus()
{
    if (editPlus_) return;
    editPlus_ = true;

    add(
        View{}
        .bottomLeft([this]{ return Size{dp(16), height()}; })
        .add(
            Label{&statusView_}
            .text([this]{ return status(); })
            .font([this]{ return style().defaultSmallFont(); })
            .baselineEnd([this]{ return Point{width() - dp(24), dp(16)}; })
            .color([this]{ return theme().secondaryTextColor(); })
            .paper([this]{ return basePaper(); })
            .visible([this]{ return status() != ""; })
        )
        .add(
            Text{&helpView_}
            .text([this]{ return help(); })
            .font([this]{ return style().defaultSmallFont(); })
            .maxWidth([this]{ return width() - dp(28) - (statusView_.visible() ? statusView_.width() + dp(8) : 0); })
            .baselineStart([]{ return Point{0, dp(16)}; })
            .color([this]{ return theme().secondaryTextColor(); })
            .paper([this]{ return basePaper(); })
            .visible([this]{ return help() != "" && error() == ""; })
        )
        .add(
            Text{&errorView_}
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

Control TextField::State::delegate() const
{
    return input_;
}

Size TextField::State::preferredSize() const
{
    Size s = input_.preferredSize() + Size{dp(28), dp(32) + messageHeight()}.ceil();
    s[0] = expandableWidth(s[0]);
    return s;
}

Size TextField::State::minSize() const
{
    return input_.minSize() + Size{dp(28), dp(32)}.ceil();
}

Size TextField::State::maxSize() const
{
    return input_.maxSize() + Size{dp(28), dp(32)}.ceil();
}

TextField::TextField():
    Control{onDemand<State>}
{}

TextField::TextField(Out<TextField> self):
    Control{new State}
{
    self = weak<TextField>();
}

TextField::TextField(const String &label, Out<TextField> self):
    Control{new State{label}}
{
    self = weak<TextField>();
}

TextField::TextField(const TextInput &input, const String &title):
    Control{new State{input, title}}
{}

String TextField::title() const
{
    return me().title();
}

TextField &TextField::title(const String &newValue)
{
    me().title(newValue);
    return *this;
}

String TextField::text() const
{
    return me().text();
}

TextField &TextField::text(const String &newValue)
{
    me().input_.text(newValue);
    return *this;
}

String TextField::placeholder() const
{
    return me().placeholder();
}

TextField &TextField::placeholder(const String &newValue)
{
    me().placeholder(newValue);
    return *this;
}

String TextField::help() const
{
    return me().help();
}

TextField &TextField::help(const String &newValue)
{
    me().editPlus();
    me().help(newValue);
    return *this;
}

TextField &TextField::help(Definition<String> &&f)
{
    me().editPlus();
    me().help(std::move(f));
    return *this;
}

String TextField::error() const
{
    return me().error();
}

TextField &TextField::error(const String &newValue)
{
    me().editPlus();
    me().error(newValue);
    return *this;
}

TextField &TextField::error(Definition<String> &&f)
{
    me().editPlus();
    me().error(std::move(f));
    return *this;
}

String TextField::status() const
{
    return me().status();
}

TextField &TextField::status(const String &newValue)
{
    me().editPlus();
    me().status(newValue);
    return *this;
}

TextField &TextField::status(Definition<String> &&f)
{
    me().editPlus();
    me().status(std::move(f));
    return *this;
}

TextField &TextField::icon(Ideographic ch)
{
    me().setLeading(Icon{ch});
    return *this;
}

TextField &TextField::alertIcon()
{
    me().setAlertIcon();
    return *this;
}

TextField &TextField::leading(const View &view)
{
    me().setLeading(view);
    return *this;
}

TextField &TextField::trailing(const View &view)
{
    me().setTrailing(view);
    return *this;
}

TextField &TextField::onAccepted(Call<void()> &&f)
{
    me().input_.onAccepted(std::move(f));
    return *this;
}

TextField &TextField::onRejected(Call<void()> &&f)
{
    me().input_.onRejected(std::move(f));
    return *this;
}

TextField &TextField::accept(Call<bool()> &&f)
{
    me().input_.accept(std::move(f));
    return *this;
}

TextField &TextField::filter(Call<bool(Range range, InOut<String>)> &&f)
{
    me().input_.filter(std::move(f));
    return *this;
}

TextInput TextField::input() const
{
    return me().input_;
}

TextField::State &TextField::me()
{
    return View::me().as<State>();
}

const TextField::State &TextField::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
