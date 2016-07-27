/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

class Variant;

/** \class Color Color.h cc/Color
  * \brief RGB color tuple
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

    Color()
    {
        bytes_[AlphaIndex] = 0x00;
        bytes_[BlueIndex]  = 0xFF;
        bytes_[GreenIndex] = 0xFF;
        bytes_[RedIndex]   = 0xFF;
    }

    Color(const char *s)
    {
        bool ok = false;
        *this = parse(s, &ok);
        if (!ok) *this = Color();
    }

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
    {
        bytes_[AlphaIndex] = a;
        bytes_[BlueIndex]  = b;
        bytes_[GreenIndex] = g;
        bytes_[RedIndex]   = r;
    }

    static Color parse(const char *s, bool *ok = 0);
    inline static Color transparent() { return Color(0, 0, 0, 0); }
    inline static Color black() { return Color(0, 0, 0); }
    inline static Color white() { return Color(0xFF, 0xFF, 0xFF); }
    inline static Color red() { return Color(0xFF, 0, 0); }
    inline static Color green() { return Color(0, 0xFF, 0); }
    inline static Color blue() { return Color(0, 0, 0xFF); }

    inline bool isValid() const { return !(bytes_[AlphaIndex] == 0 && (bytes_[BlueIndex] != 0 || bytes_[GreenIndex] != 0 || bytes_[RedIndex] != 0)); }

    inline static uint8_t &red(Color &c) { return c.bytes_[RedIndex]; }
    inline static uint8_t &green(Color &c) { return c.bytes_[GreenIndex]; }
    inline static uint8_t &blue(Color &c) { return c.bytes_[BlueIndex]; }
    inline static uint8_t &alpha(Color &c) { return c.bytes_[AlphaIndex]; }

    inline operator uint32_t() { return word_; }

    inline bool operator==(const Color &b) const { return word_ == b.word_; }
    inline bool operator!=(const Color &b) const { return word_ != b.word_; }

    static Color blend(Color a, Color b);

protected:
    friend class Variant;

    inline static Color cast(uint32_t x) { Color c; c.word_ = x; return c; }

    union {
        uint8_t bytes_[4];
        uint32_t word_;
    };
};

String str(Color c);

} // namespace cc
