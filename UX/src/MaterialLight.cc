/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MaterialLight>
#include <cc/color/Material>

namespace cc {

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
    Color windowColor() const override { return Material::Grey50; }
    Color popupColor() const override { return Color::White; }

    Color appBarColor(bool pressed) const override { return primaryDarkColor().lighter(pressed ? 10 : 0); }
    Color appBarTextColor() const override { return Color::White; }
    Color appBarShadowColor() const override { return 0xA0000000u; }

    Color dialogShadowColor() const override { return 0x50000000u; }

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

    Color inputFieldFillColor() const override { return windowColor().darker(7); }
    Color inputFieldFillColor(bool pressed) const override { return windowColor().darker(pressed ? 10 : 7); }
    Color itemHighlightColor() const override { return windowColor().darker(7); }

    Color sliderKnobColor() const override { return primaryDarkColor(); }
    Color sliderKnobShadowColor() const override { return 0x30000000u; }
    Color sliderActiveTrackColor() const override { return primaryDarkColor(); }
    Color sliderInactiveTrackColor() const override { return windowColor().mixedWith(secondaryDarkColor(), 38); }
    Color sliderFocusColor() const override { return primaryColor(); }

    Color switchKnobColor(bool on) const override { return on ? primaryDarkColor() : Color{Material::White}; }
    Color switchKnobShadowColor(bool on) const override { return sliderKnobShadowColor(); }
    Color switchTrackColor(bool on) const override { return on ? windowColor().mixedWith(secondaryDarkColor(), 38) : Color{Material::Grey500}; }
    Color switchFocusColor() const override { return primaryColor(); }

    Color checkboxColor(bool on) const override { return on ? primaryDarkColor() : secondaryTextColor(); }
    Color radioboxColor(bool on) const override { return on ? primaryDarkColor() : secondaryTextColor(); }

    Color pushButtonColor(PushButtonStyle style) const override
    {
        return (style == PushButtonStyle::Elevated) ? Color{Material::Blue50} : primaryDarkColor();
    }

    Color pushButtonTextColor(PushButtonStyle style) const override
    {
        return (style == PushButtonStyle::Elevated) ? Color{Material::Blue900} : Color::White;
    }

    Color pushButtonShadowColor(PushButtonStyle style) const override { return 0x60000000u; }

    Color touchButtonColor(TouchButtonStyle style, bool pressed) const override
    {
        if (style == TouchButtonStyle::Filled) {
            return primaryDarkColor().darker(pressed ? 10 : 0);
        }
        else if (style == TouchButtonStyle::Tonal) {
            return windowColor().mixedWith(secondaryDarkColor(), 38).darker(pressed ? 10 : 0);
        }
        else if (style == TouchButtonStyle::Outline || style == TouchButtonStyle::Text) {
            if (pressed) {
                return itemHighlightColor();
            }
        }
        return Color{};
    }

    Color touchButtonTextColor(TouchButtonStyle style, bool pressed) const override
    {
        if (style == TouchButtonStyle::Outline || style == TouchButtonStyle::Text) {
            return primaryDarkColor();
        }
        else if (style == TouchButtonStyle::Tonal) {
            return primaryTextColor();
        }
        return Color::White;
    }
};

MaterialLight::MaterialLight():
    Theme{new State}
{}

} // namespace cc
