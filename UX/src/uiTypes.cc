/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/uiTypes>
#include <cc/DisplayManager>
#include <cc/Application>
#include <cc/Unicode>

namespace cc {

bool textWrapBehindDefault(const String &text, long byteOffset, long byteCount)
{
    assert(byteOffset + byteCount > 0);
    assert(byteCount > 0);

    uint8_t ch = text.byteAt(byteOffset + byteCount - 1);
    uint8_t ch2 = text.has(byteOffset + byteCount) ? text.byteAt(byteOffset + byteCount) : 0;

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
        char32_t ch = *Unicode{text.chars() + byteOffset, text.count() - byteOffset}.begin();

        canWrap =
            ((0x4E00 <= ch && ch <= 0x9FEA) || (0xF900 <= ch && ch <= 0xFAFF) /* CJK Unified */) ||
            (0x3000 <= ch && ch <= 0x30FF /* Hiragana, Katakana */) ||
            (0x2F00 <= ch && ch <= 0x2FDF /* Kangxi Radicals */);
    }

    return canWrap;
}

double dp(double x)
{
    return DisplayManager{}.displayDensityRatio() * x * Application{}.textZoom();
}

double sp(double x)
{
    return std::ceil(dp(x));
}

double gu(int n)
{
    return style().gridUnit() * n;
}

} // namespace cc
