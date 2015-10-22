/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_COLOR_H
#define FLUX_COLOR_H

#include <flux/String>

namespace flux {

class Variant;

/** \brief RGB color tuple
  */
class Color
{
public:
    enum {
        AlphaIndex = 0,
        BlueIndex  = 1,
        GreenIndex = 2,
        RedIndex   = 3,
        FirstComponentIndex = BlueIndex,
        LastComponentIndex = RedIndex
    };

    static Color parse(const char *s, bool *ok = 0);

    inline static Color black() { return Color(0, 0, 0, 0xFF); }

    Color(): word_(0) {}

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
    {
        bytes_[AlphaIndex] = a;
        bytes_[BlueIndex]  = b;
        bytes_[GreenIndex] = g;
        bytes_[RedIndex]   = r;
    }

    inline static uint8_t &red(Color &c) { return c.bytes_[RedIndex]; }
    inline static uint8_t &green(Color &c) { return c.bytes_[GreenIndex]; }
    inline static uint8_t &blue(Color &c) { return c.bytes_[BlueIndex]; }
    inline static uint8_t &alpha(Color &c) { return c.bytes_[AlphaIndex]; }

    inline operator uint32_t() { return word_; }

    inline bool operator==(const Color &b) const { return word_ == b.word_; }
    inline bool operator!=(const Color &b) const { return word_ != b.word_; }

    /** alpha blend color a over b
      */
    inline static Color blend(Color a, Color b)
    {
        uint32_t o_a = Color::alpha(a);
        if (o_a == 0xFF) return a;

        uint32_t o_b = Color::alpha(b);
        o_a += (o_a >= 0x80);
        o_b += (o_b >= 0x80);
        uint32_t n_a = 0x100 - o_a;

        if (o_b == 0x100) {
            for (int i = FirstComponentIndex; i <= LastComponentIndex; ++i)
                b.bytes_[i] = (o_a * a.bytes_[i] + n_a * b.bytes_[i]) >> 8;
        }
        else {
            o_a <<= 8;
            n_a *= o_b;
            uint32_t h = o_a + n_a;
            for (int i = FirstComponentIndex; i <= LastComponentIndex; ++i)
                b.bytes_[i] = (o_a * a.bytes_[i] + n_a * b.bytes_[i]) / h;
            Color::alpha(b) = h >> 16;
        }

        return b;
    }

protected:
    friend class Variant;

    inline static Color cast(uint32_t x) { Color c; c.word_ = x; return c; }

    union {
        uint8_t bytes_[4];
        uint32_t word_;
    };
};

String str(Color c);

} // namespace flux

#endif // FLUX_COLOR_H
