/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Theme>

namespace cc {

Color Theme::State::appBarTextColor() const
{
    return titleTextColor();
}

Color Theme::State::dialogScrimColor() const
{
    return Color::Black(32);
}

Color Theme::State::dialogColor() const
{
    return popupColor();
}

Color Theme::State::inputFieldFillColor(bool pressed) const
{
    return inputFieldFillColor();
}

Color Theme::State::checkboxColor(bool on) const
{
    return on ? primaryColor() : secondaryTextColor();
}

Color Theme::State::checkboxFocusColor() const
{
    return switchFocusColor();
}

Color Theme::State::radioboxColor(bool on) const
{
    return on ? primaryColor() : secondaryTextColor();
}

Color Theme::State::radioboxFocusColor() const
{
    return switchFocusColor();
}

Color Theme::State::pushButtonFocusColor(PushButtonStyle style) const
{
    return Color{};
}

Color Theme::State::touchButtonTextColor(TouchButtonStyle style, bool pressed) const
{
    if (style == TouchButtonStyle::Outline) {
        return primaryColor();
    }
    if (style == TouchButtonStyle::Tonal) {
        return primaryTextColor();
    }
    return titleTextColor();
}

Pen Theme::State::touchButtonBorder(TouchButtonStyle style, bool pressed) const
{
    if (style == TouchButtonStyle::Outline) {
        return Pen{secondaryTextColor(), sp(1)};
    }
    return Pen{};
}

Color Theme::State::flickableIndicatorColor() const
{
    return inputFieldFillColor();
}

Color Theme::State::flickableIndicatorHandleColor() const
{
    return sliderKnobColor();
}

Color Theme::State::clockDialClockFaceColor(bool pressed) const
{
    return inputFieldFillColor();
}

Color Theme::State::clockDialClockFaceTextColor(bool pressed) const
{
    return primaryTextColor();
}

Color Theme::State::clockDialCurrentValueColor(bool pressed) const
{
    return sliderKnobColor();
}

Color Theme::State::clockDialCurrentValueTextColor(bool pressed) const
{
    return titleTextColor();
}

double Theme::State::textCursorWidth() const
{
    return dp(1);
}

} // namespace cc
