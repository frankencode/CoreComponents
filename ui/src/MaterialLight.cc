/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/MaterialLight>
#include <cc/colors/Material>

namespace cc::ui {

using color::Material;

struct MaterialLight::State: public Theme::State
{
    Color primaryColor() const override { return Material::Blue500; }
    Color primaryDarkColor() const { return 0x0069c0u; }
    Color primaryLightColor() const { return 0x6ec6ffu; }
    Color secondaryColor() const override { return Material::Lime500; }
    Color secondaryDarkColor() const { return 0x99aa00u; }
    Color secondaryLightColor() const { return 0xffff6eu; }
    Color alertColor() const override { return Material::RedA700; }

    Color statusBarColor() const override { return Material::Grey300; }
    Color appBarColor() const override { return Material::Grey100; }
    Color windowColor() const override { return Material::Grey50; }
    Color popupColor() const override { return Color::White; }

    Color titleTextColor() const override { return Color::White; }
    Color focusTextColor() const override { return primaryDarkColor()(87); }
    Color primaryTextColor() const override { return Color::Black(87); }
    Color secondaryTextColor() const override { return Color::Black(54); }
    Color inactiveTextColor() const override { return Color::Black(38); }

    Color inputLineColor() const override { return Color::Black(42); }
    Color hoverInputLineColor() const override { return primaryTextColor(); }
    Color pressedInputLineColor() const override { return primaryDarkColor(); }
    Color focusInputLineColor() const override { return primaryDarkColor(); }
    Color textCursorColor() const override { return primaryColor(); }
    Color textSelectionColor() const override { return primaryTextColor(); }
    Color textSelectionPaper() const override { return secondaryLightColor(); }

    Color dividerColor() const override { return Color::Black(21); }

    Color inputFieldFillColor() const override { return windowColor().darker(5); }
    Color inputFieldFillColor(bool pressed) const override { return windowColor().darker(pressed ? 10 : 5); }
    Color itemHighlightColor() const override { return windowColor().darker(5); }

    Color sliderKnobColor() const override { return primaryDarkColor(); }
    Color sliderKnobShadowColor() const override { return 0x30000000u; }
    Color sliderActiveTrackColor() const override { return primaryDarkColor(); }
    Color sliderInactiveTrackColor() const override { return windowColor().mixedWith(secondaryDarkColor(), 38); }
    Color sliderFocusColor() const override { return primaryColor(); }

    Color switchKnobColor(bool on) const override { return on ? primaryDarkColor() : Color{Material::White}; }
    Color switchKnobShadowColor(bool on) const override { return sliderKnobShadowColor(); }
    Color switchTrackColor(bool on) const override { return on ? windowColor().mixedWith(secondaryDarkColor(), 38) : Color{Material::Grey500}; }
    Color switchFocusColor() const override { return primaryColor(); }

    Color buttonColor(bool pressed) const override { return primaryDarkColor().darker(pressed ? 20 : 0); }
    Color buttonFocusColor(bool pressed) const override { return buttonColor(pressed).darker(20); }
    Color buttonTextColor(bool pressed) const override { return Color::White; }
    Color buttonTextFocusColor(bool pressed) const override { return Color::White; }

    Color elevatedButtonColor(bool pressed) const override { return Color{Material::Blue50}.darker(pressed ? 5 : 0); }
    Color elevatedButtonFocusColor(bool pressed) const override { return elevatedButtonColor(pressed).darker(5); }
    Color elevatedButtonTextColor(bool pressed) const override { return Material::Blue900; }
    Color elevatedButtonFocusTextColor(bool pressed) const override { return Material::Blue900; }
    Color elevatedButtonShadowColor() const override { return 0x60000000u; }
};

MaterialLight::MaterialLight():
    Theme{new State}
{}

} // namespace cc::ui