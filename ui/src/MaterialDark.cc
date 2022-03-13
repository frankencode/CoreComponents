/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/MaterialDark>
#include <cc/color/Material>

namespace cc::ui {

using color::Material;

struct MaterialDark::State: public Theme::State
{
    Color primaryColor() const override { return Material::Purple500; }
    Color secondaryColor() const override { return Material::Cyan500; }
    Color alertColor() const override { return Material::RedA400; }

    Color statusBarColor() const override { return Material::Black; }
    Color appBarColor() const override { return Material::Grey900; }
    Color windowColor() const override { return 0x303030; }
    Color popupColor() const override { return Material::Grey800; }

    Color titleTextColor() const override { return Color::White; }
    Color focusTextColor() const override { return primaryColor(); }
    Color primaryTextColor() const override { return Color::White; }
    Color secondaryTextColor() const override { return Color::White(70); }
    Color inactiveTextColor() const override { return Color::White(50); }

    Color inputLineColor() const override { return secondaryTextColor(); }
    Color hoverInputLineColor() const override { return primaryTextColor(); }
    Color pressedInputLineColor() const override { return primaryColor(); }
    Color focusInputLineColor() const override { return primaryColor(); }
    Color textCursorColor() const override { return primaryColor(); }
    Color textSelectionColor() const override { return primaryTextColor(); }
    Color textSelectionPaper() const override { return secondaryColor(); }

    Color dividerColor() const override { return Color::White(20); }

    Color inputFieldFillColor() const override { return windowColor().lighter(4); }
    Color inputFieldFillColor(bool pressed) const override { return windowColor().lighter(pressed ? 9 : 4); }
    Color itemHighlightColor() const override { return windowColor().lighter(4); }

    Color sliderKnobColor () const override { return primaryColor(); }
    Color sliderKnobShadowColor() const override { return 0x30FFFFFFu; }
    Color sliderActiveTrackColor() const override { return primaryColor(); }
    Color sliderInactiveTrackColor() const override { return windowColor().mixedWith(secondaryColor(), 38); }
    Color sliderFocusColor() const override { return primaryColor(); }

    Color switchKnobColor(bool on) const override { return on ? sliderKnobColor() : Color{Material::White}; }
    Color switchKnobShadowColor(bool on) const override { return sliderKnobShadowColor(); }
    Color switchTrackColor(bool on) const override { return on ? windowColor().mixedWith(secondaryColor(), 38) : Color{Material::Grey500}; }
    Color switchFocusColor() const override { return primaryColor(); }

    Color buttonColor(bool pressed) const override { return primaryColor().lighter(pressed ? 10 : 0); }
    Color buttonFocusColor(bool pressed) const override { return buttonColor(pressed).lighter(10); }
    Color buttonTextColor(bool pressed) const override { return Color::White; }
    Color buttonTextFocusColor(bool pressed) const override { return Color::White; }

    Color elevatedButtonColor(bool pressed) const override { return secondaryColor().lighter(pressed ? 10 : 0); }
    Color elevatedButtonFocusColor(bool pressed) const override { return elevatedButtonColor(pressed).lighter(10); }
    Color elevatedButtonTextColor(bool pressed) const override { return Color::White; }
    Color elevatedButtonFocusTextColor(bool pressed) const override { return Color::White; }
    Color elevatedButtonShadowColor() const override { return sliderKnobShadowColor(); }
};

MaterialDark::MaterialDark():
    Theme{new State}
{}

} // namespace cc::ui
