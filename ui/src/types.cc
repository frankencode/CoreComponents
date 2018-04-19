/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <cc/str>
#include <cc/Utf8Walker>
#include <cc/ui/DisplayManager>
#include <cc/ui/Application>
#include <cc/ui/types>

namespace cc {
namespace ui {

String str(TextSmoothing smoothing)
{
    switch (smoothing) {
        case TextSmoothing::Default     : return "TextSmoothing::Default";
        case TextSmoothing::None        : return "TextSmoothing::None";
        case TextSmoothing::Grayscale   : return "TextSmoothing::Grayscale";
        case TextSmoothing::RgbSubpixel : return "TextSmoothing::RgbSubpixel";
        case TextSmoothing::BgrSubpixel : return "TextSmoothing::BgrSubpixel";
        case TextSmoothing::VrgbSubpixel: return "TextSmoothing::VrgbSubpixel";
        case TextSmoothing::VbgrSubpixel: return "TextSmoothing::VbgrSubpixel";
    }

    return String{};
}

String str(Slant slant)
{
    switch (slant) {
        case Slant::Normal : return "Slant::Normal";
        case Slant::Italic : return "Slant::Italic";
        // case Slant::Oblique: return "Oblique";
    }

    return String{};
}

String str(Stretch stretch)
{
    switch (stretch) {
        case Stretch::UltraCondensed: return "Stretch::UltraCondensed";
        case Stretch::ExtraCondensed: return "Stretch::ExtraCondensed";
        case Stretch::Condensed     : return "Stretch::Condensed";
        case Stretch::SemiCondensed : return "Stretch::SemiCondensed";
        case Stretch::Normal        : return "Stretch::Normal";
        case Stretch::SemiExpanded  : return "Stretch::SemiExpanded";
        case Stretch::Expanded      : return "Stretch::Expaned";
        case Stretch::ExtraExpanded : return "Stretch::ExtraExpanded";
        case Stretch::UltraExpanded : return "Stretch::UltraExpanded";
    }

    return String{};
}

String str(Weight weight)
{
    switch (weight) {
        case Weight::Thin      : return "Weight::Thin";
        case Weight::ExtraLight: return "Weight::ExtraLight";
        case Weight::Light     : return "Weight::Light";
        case Weight::Normal    : return "Weight::Normal";
        case Weight::Medium    : return "Weight::Medium";
        case Weight::SemiBold  : return "Weight::SemiBold";
        case Weight::Bold      : return "Weight::Bold";
        case Weight::ExtraBold : return "Weight::ExtraBold";
        case Weight::Black     : return "Weight::Black";
    }

    return String{};
}

String str(Pitch pitch)
{
    switch (pitch) {
        case Pitch::Fixed:    return "Pitch::Fixed";
        case Pitch::Variable: return "Pitch::Variable";
    }

    return String{};
}

String str(TextAlign align)
{
    switch (align) {
        case TextAlign::Left:    return "TextAlign::Left";
        case TextAlign::Right:   return "TextAlign::Right";
        case TextAlign::Center:  return "TextAlign::Center";
        case TextAlign::Justify: return "TextAlign::Justify";
    }

    return String{};
}

String str(Decoration decoration)
{
    if (decoration == Decoration::None) return "Decoration::None";
    Ref<StringList> parts = StringList::create();
    if (+(decoration & Decoration::Underline)) parts << "Decoration::Underline";
    if (+(decoration & Decoration::StrikeOut)) parts << "Decoration::StrikeOut";
    return parts->join("|");
}

String str(ColumnAlign align)
{
    switch (align) {
        case ColumnAlign::Auto:   return "ColumnAlign::Auto";
        case ColumnAlign::Left:   return "ColumnAlign::Left";
        case ColumnAlign::Right:  return "ColumnAlign::Right";
        case ColumnAlign::Center: return "ColumnAlign::Center";
    }

    return String{};
}

String str(RowAlign align)
{
    switch (align) {
        case RowAlign::Top:    return "RowAlign::Top";
        case RowAlign::Bottom: return "RowAlign::Bottom";
        case RowAlign::Center: return "RowAlign::Center";
    }

    return String{};
}

String str(MouseButton mask)
{
    Ref<StringList> parts = StringList::create();
    if (+(mask & MouseButton::Left))  parts << "MouseButton::Left";
    if (+(mask & MouseButton::Right)) parts << "MouseButton::Right";
    if (+(mask & MouseButton::X1))    parts << "MouseButton::X1";
    if (+(mask & MouseButton::X2))    parts << "MouseButton::X2";
    if (+(mask & MouseButton::Middle) == +MouseButton::Middle)
        parts << "MouseButton::Middle";
    return parts->join("|");
}

String str(PointerAction action)
{
    switch (action) {
        case PointerAction::Moved   : return "PointerAction::Moved";
        case PointerAction::Pressed : return "PointerAction::Pressed";
        case PointerAction::Released: return "PointerAction::Released";
    };

    return String{};
}

String str(KeyAction action)
{
    switch (action) {
        case KeyAction::Pressed : return "KeyAction::Pressed";
        case KeyAction::Released: return "KeyAction::Released";
    };

    return String{};
}

String str(CursorShape shape)
{
    switch (shape) {
        case CursorShape::Arrow                   : return "CursorShape::Arrow";
        case CursorShape::IBeam                   : return "CursorShape::IBeam";
        case CursorShape::Wait                    : return "CursorShape::Wait";
        case CursorShape::CrossHair               : return "CursorShape::CrossHair";
        case CursorShape::WaitArrow               : return "CursorShape::WaitArrow";
        case CursorShape::ResizeNorthWestSouthEast: return "CursorShape::ResizeNorthWestSouthEast";
        case CursorShape::ResizeNorthEastSouthWest: return "CursorShape::ResizeNorthEastSouthWest";
        case CursorShape::ResizeWestEast          : return "CursorShape::ResizeWestEast";
        case CursorShape::ResizeNorthSouth        : return "CursorShape::ResizeNorthSouth";
        case CursorShape::ResizeCross             : return "CursorShape::ResizeCross";
        case CursorShape::Forbidden               : return "CursorShape::Forbidden";
        case CursorShape::Hand                    : return "CursorShape::Hand";
    };

    return String{};
}

bool textWrapBehindDefault(String text, int byteOffset, int byteCount)
{
    uint8_t ch = text->byteAt(byteOffset + byteCount - 1);
    uint8_t ch2 = text->has(byteOffset + byteCount) ? text->byteAt(byteOffset + byteCount) : 0;

    if (
        (ch  == '(' || ch  == '{' || ch  == '[') ||
        (ch2 == ')' || ch2 == '}' || ch2 == ']')
    ) return false;

    bool canWrap = (
        (ch <= 0x2F               /* !"#$%&\'()*+,-./ */) ||
        (0x3A <= ch && ch <= 0x3F /* :;<=>?           */) ||
        (0x5B <= ch && ch <= 0x60 /* [\]^_`           */) ||
        (0x7B <= ch && ch <= 0x7E /* {|}~             */)
    ) && (ch2 < 0x20 || 0x2F < ch2);


    if (!canWrap && byteCount > 1) {
        uchar_t ch = Utf8Walker(text->chars() + byteOffset)->getChar();

        canWrap =
            ((0x4E00 <= ch && ch <= 0x9FEA) || (0xF900 <= ch && ch <= 0xFAFF) /* CJK Unified */) ||
            (0x3000 <= ch && ch <= 0x30FF /* Hiragana, Katakana */) ||
            (0x2F00 <= ch && ch <= 0x2FDF /* Kangxi Radicals */);
    }

    return canWrap;
}

double degrees(double angle)
{
    return M_PI * angle / 180;
}

double dp(double x)
{
    return std::ceil(DisplayManager::instance()->displayDensityRatio() * x);
}

double sp(double x)
{
    return std::ceil(dp(x) * Application::instance()->textZoom());
}

}} // namespace cc::ui
