/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ButtonState>
#include <cc/ui/Icon>

namespace cc::ui {

Button::State::State()
{
    preferredHeight([this]{ return sp(40); });

    radius([this]{ return preferredHeight() / 2; });

    leadingSpacing([this]{
        return radius();
    });

    innerSpacing([this]{ return sp(10); });

    trailingSpacing([this]{
        if (label() && icon()) return sp(5) + radius();
        if (!label() || label().text() == "") return radius();
        return radius();
    });

    add(
        Box{&box_}
        .radius([this]{ return radius(); })
        .color([this]{
            return focus() ?
                theme().buttonFocusColor(pressed()) :
                theme().buttonColor(pressed());
        })
        .size([this]{ return size(); })
    );

    size([this]{ return preferredSize(); });

    padding([this]{ return Padding{sp(8)}; });

    onKeyPressed([this](const KeyEvent &event)
    {
        if (
            event.keyCode() == KeyCode::Key_Return ||
            event.keyCode() == KeyCode::Key_Space
        ) {
            pressedOverwrite(true);
            return true;
        }
        return false;
    });

    onKeyReleased([this](const KeyEvent &event)
    {
        if (
            event.keyCode() == KeyCode::Key_Return ||
            event.keyCode() == KeyCode::Key_Space
        ) {
            pressedOverwrite(false);
            if (accept()) {
                onAccepted();
                gotoNext();
            }
            return true;
        }
        return false;
    });
}

void Button::State::setIcon(View view)
{
    if (icon() == view) return;
    if (icon()) remove(icon());
    view
        .centerLeft([this]{ return Point{leadingSpacing(), height() / 2 - 1}; })
        .paper([this]{ return box_.color(); });
    add(view);
    icon(view);
}

void Button::State::setText(const String &newValue)
{
    if (label()) {
        label().text(newValue);
    }
    else {
        Label newLabel;
        newLabel
            .color([this]{
                return focus() ?
                    theme().buttonTextColor(pressed()) :
                    theme().buttonTextFocusColor(pressed());
            })
            .text(newValue)
            .paper([this]{ return box_.color(); });
        label(newLabel);
        label().pos([this]{
            return Point{
                leadingSpacing() + (icon() ? icon().width() + innerSpacing() : 0.),
                (height() - label().height()) / 2 - 1
            };
        });
        add(label());
    }
}

void Button::State::defineText(Definition<String> &&f)
{
    if (label()) {
        label().text(std::move(f));
    }
    else {
        Label newLabel;
        newLabel
            .color([this]{
                return focus() ?
                    theme().buttonTextColor(pressed()) :
                    theme().buttonTextFocusColor(pressed());
            })
            .text(std::move(f))
            .paper([this]{ return box_.color(); });
        label(newLabel);
        label().pos([this]{
            return Point{
                leadingSpacing() + (icon() ? icon().width() + innerSpacing() : 0.),
                (height() - label().height()) / 2 - 1
            };
        });
        add(label());
    }
}

Size Button::State::preferredSize() const
{
    return Size{
        leadingSpacing() +
        (icon() ? icon().width() : 0.) +
        ((icon() && label() && label().text() != "") ? innerSpacing() : 0.) +
        (label() ? label().width() : 0.) +
        trailingSpacing(),
        preferredHeight()
    };
}

Size Button::State::minSize() const
{
    return preferredSize();
}

Size Button::State::maxSize() const
{
    return Size{ std::numeric_limits<double>::max(), sp(40) };
}

Button::Button():
    InputControl{onDemand<State>}
{}

Button::Button(Out<Button> self):
    InputControl{new State}
{
    self = weak<Button>();
}

String Button::text() const
{
    return me().label() ? me().label().text() : String{};
}

Button &Button::text(const String &newValue)
{
    me().setText(newValue);
    return *this;
}

Button &Button::text(Definition<String> &&f)
{
    me().defineText(std::move(f));
    return *this;
}

Button &Button::icon(Ideographic ch, double size)
{
    me().setIcon(Icon{ch, size > 0 ? size : sp(20), textColor()});
    return *this;
}

Button &Button::icon(const View &view)
{
    me().setIcon(view);
    return *this;
}

Color Button::color() const
{
    return me().box_.color();
}

Button &Button::color(Color newValue)
{
    me().box_.color(newValue);
    return *this;
}

Button &Button::color(Definition<Color> &&f)
{
    me().box_.color(std::move(f));
    return *this;
}

Color Button::textColor() const
{
    if (!me().label()) return theme().buttonTextColor(pressed());
    return me().label().color();
}

Button &Button::textColor(Color newValue)
{
    me().label().color(newValue);
    return *this;
}

Button &Button::textColor(Definition<Color> &&f)
{
    me().label().color(std::move(f));
    return *this;
}

double Button::radius() const
{
    return me().radius();
}

Button &Button::radius(double newValue)
{
    me().radius(newValue);
    return *this;
}

Button &Button::radius(Definition<double> &&f)
{
    me().radius(std::move(f));
    return *this;
}

double Button::leadingSpacing() const
{
    return me().leadingSpacing();
}

Button &Button::leadingSpacing(double newValue)
{
    me().leadingSpacing(newValue);
    return *this;
}

Button &Button::leadingSpacing(Definition<double> &&f)
{
    me().leadingSpacing(std::move(f));
    return *this;
}

double Button::trailingSpacing() const
{
    return me().trailingSpacing();
}

Button &Button::trailingSpacing(double newValue)
{
    me().trailingSpacing(newValue);
    return *this;
}

Button &Button::trailingSpacing(Definition<double> &&f)
{
    me().trailingSpacing(std::move(f));
    return *this;
}

double Button::innerSpacing() const
{
    return me().innerSpacing();
}

Button &Button::innerSpacing(double newValue)
{
    me().innerSpacing(newValue);
    return *this;
}

Button &Button::innerSpacing(Definition<double> &&f)
{
    me().innerSpacing(std::move(f));
    return *this;
}

Button &Button::preferredHeight(double newValue)
{
    me().preferredHeight(newValue);
    return *this;
}

Button &Button::preferredHeight(Definition<double> &&f)
{
    me().preferredHeight(std::move(f));
    return *this;
}

Button &Button::decorate(const View &newValue)
{
    me().box_.decorate(newValue);
    return *this;
}

Button &Button::decorate(Definition<View> &&f)
{
    me().box_.decorate(std::move(f));
    return *this;
}

Button::State &Button::me()
{
    return View::me().as<State>();
}

const Button::State &Button::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
