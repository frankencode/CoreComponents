/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
  * \ingroup misc
  * \brief RGB color tuple
  * \see ColorNames
  */
class Color
{
public:
    /// %Color channels
    enum {
        AlphaChannel = 0,           ///< Index of alpha channel
        BlueChannel  = 1,           ///< Index of blue channel
        GreenChannel = 2,           ///< Index of green channel
        RedChannel   = 3,           ///< Index of red channel
        FirstChannel = BlueChannel, ///< Index of first channel
        LastChannel = RedChannel    ///< Index of last channel
    };

    /// Default constructor
    Color()
    {
        bytes_[AlphaChannel] = 0x00;
        bytes_[BlueChannel]  = 0xFF;
        bytes_[GreenChannel] = 0xFF;
        bytes_[RedChannel]   = 0xFF;
    }

    /** Construct a color from a color literal or name \a s.
      * %Color literals of the format \c #RRGGBB, \c #RRGGBBAA, \c #RGB, \c #RGAB and
      * SVG color names are allowed (e.g. \c "AliceBlue").
      */
    Color(const char *s)
    {
        bool ok = false;
        *this = parse(s, &ok);
        if (!ok) *this = Color();
    }

    /** Construct a color from an color tuple
      * \param r red component (0..255)
      * \param g green component (0..255)
      * \param b blue component (0..255)
      * \param a alpha value / opacity (0..255)
      */
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
    {
        bytes_[AlphaChannel] = a;
        bytes_[BlueChannel]  = b;
        bytes_[GreenChannel] = g;
        bytes_[RedChannel]   = r;
    }

    /** \copydoc Color(const char *)
      * \param ok return true if \a s is a valid color literal or name
      */
    static Color parse(const char *s, bool *ok = 0);

    /// Get a transparent color
    inline static Color transparent() { return Color(0, 0, 0, 0); }

    /// Get a black color
    inline static Color black() { return Color(0, 0, 0); }

    /// Get a white color
    inline static Color white() { return Color(0xFF, 0xFF, 0xFF); }

    /// Get a red color
    inline static Color red() { return Color(0xFF, 0, 0); }

    /// Convenience function to get a green color
    inline static Color green() { return Color(0, 0xFF, 0); }

    /// Convenience function to get a blue color
    inline static Color blue() { return Color(0, 0, 0xFF); }

    /// Check if color \c is valid
    inline static bool isValid(Color &c) { return !(c.bytes_[AlphaChannel] == 0 && (c.bytes_[BlueChannel] != 0 || c.bytes_[GreenChannel] != 0 || c.bytes_[RedChannel] != 0)); }

    /// Obtain red component of \a c
    inline static uint8_t &red(Color &c) { return c.bytes_[RedChannel]; }

    /// Obtain green component of \a c
    inline static uint8_t &green(Color &c) { return c.bytes_[GreenChannel]; }

    /// Obtain blue component of \a c
    inline static uint8_t &blue(Color &c) { return c.bytes_[BlueChannel]; }

    /// Obtain alpha component of \a c
    inline static uint8_t &alpha(Color &c) { return c.bytes_[AlphaChannel]; }

    /// Type conversion to uint32_t operator
    inline operator uint32_t() { return word_; }

    /// Compare two colors for equality
    inline bool operator==(const Color &b) const { return word_ == b.word_; }

    /// Compare two colors for inequality
    inline bool operator!=(const Color &b) const { return word_ != b.word_; }

    /// Alpha-blend color \a a over color \a b
    static Color blend(Color a, Color b);

private:
    friend class Variant;

    inline static Color cast(uint32_t x) { Color c; c.word_ = x; return c; }

    union {
        uint8_t bytes_[4];
        uint32_t word_;
    };
};

/// Stringify a color tuple \a c
String str(Color c);

} // namespace cc
