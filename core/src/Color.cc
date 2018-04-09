/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cc/ColorNames>
#include <cc/Format>
#include <cc/colors>
#include <cc/Color>

namespace cc {

const Color Color::transparent { 0x00, 0x00, 0x00, 0x00 };
const Color Color::black       { 0x00, 0x00, 0x00 };
const Color Color::white       { 0xFF, 0xFF, 0xFF };
const Color Color::red         { 0xFF, 0x00, 0x00 };
const Color Color::green       { 0x00, 0xFF, 0x00 };
const Color Color::blue        { 0x00, 0x00, 0xFF };
const Color Color::yellow      { 0xFF, 0xFF, 0x00 };
const Color Color::cyan        { 0x00, 0xFF, 0xFF };
const Color Color::magenta     { 0xFF, 0x00, 0xFF };

Color Color::parse(const char *s, bool *ok)
{
    bool localOk;
    if (!ok) ok = &localOk;
    if (!s[0]) {
        *ok = false;
        return Color();
    }
    if (s[0] == '#') {
        struct H {
            static inline int dehex(char ch, bool *ok) {
                if ('0' <= ch && ch <= '9') return ch - '0';
                else if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
                else if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
                *ok = false;
                return -1;
            }
        };
        Color c;
        int n = 1;
        while (s[n] && n < 11) ++n;
        if (n == 7) { // #RRGGBB
            *ok = true;
            int rh = H::dehex(s[1], ok);
            int rl = H::dehex(s[2], ok);
            int gh = H::dehex(s[3], ok);
            int gl = H::dehex(s[4], ok);
            int bh = H::dehex(s[5], ok);
            int bl = H::dehex(s[6], ok);
            if (!*ok) return Color();
            return Color(
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl
            );
        }
        else if (n == 9) { // #RRGGBBAA
            *ok = true;
            int rh = H::dehex(s[1], ok);
            int rl = H::dehex(s[2], ok);
            int gh = H::dehex(s[3], ok);
            int gl = H::dehex(s[4], ok);
            int bh = H::dehex(s[5], ok);
            int bl = H::dehex(s[6], ok);
            int ah = H::dehex(s[7], ok);
            int al = H::dehex(s[8], ok);
            if (!*ok) return Color();
            return Color(
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl,
                (ah << 4) | al
            );
        }
        else if (n == 4) { // #RGB
            *ok = true;
            int rh = H::dehex(s[1], ok);
            int gh = H::dehex(s[2], ok);
            int bh = H::dehex(s[3], ok);
            if (!*ok) return Color();
            return Color(
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh
            );
        }
        else if (n == 5) { // #RGBA
            *ok = true;
            int rh = H::dehex(s[1], ok);
            int gh = H::dehex(s[2], ok);
            int bh = H::dehex(s[3], ok);
            int ah = H::dehex(s[4], ok);
            if (!*ok) return Color();
            return Color(
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh,
                (ah << 4) | ah
            );
        }
        *ok = false;
        return Color();
    }
    Color c;
    if (ColorNames::instance()->lookup(s, &c)) {
        *ok = true;
        return c;
    }
    *ok = false;
    return Color{};
}

Color Color::fromHsv(double h, double s, double v)
{
    double r, g, b;
    hsvToRgb(h, s, v, &r, &g, &b);
    return Color{
        uint32_t(std::round(0xFF * r)),
        uint32_t(std::round(0xFF * g)),
        uint32_t(std::round(0xFF * b))
    };
}

Color Color::fromHsl(double h, double s, double l)
{
    double r, g, b;
    hslToRgb(h, s, l, &r, &g, &b);
    return Color{
        uint32_t(std::round(0xFF * r)),
        uint32_t(std::round(0xFF * g)),
        uint32_t(std::round(0xFF * b))
    };
}

Color Color::blend(Color a, Color b)
{
    const uint32_t h = component<Alpha, uint32_t>(b);

    if (h == 0xFF)
    {
        uint16_t o_a = component<Alpha, uint16_t>(a);
        if (o_a == 0xFF) return a;

        uint16_t n_a = 0x100 - o_a;

        typedef uint16_t v4ui16 __attribute__((vector_size(8)));

        v4ui16 v_a {
            component<Red  , uint16_t>(a),
            component<Green, uint16_t>(a),
            component<Blue , uint16_t>(a),
            0
        };

        v4ui16 v_b {
            component<Red,   uint16_t>(b),
            component<Green, uint16_t>(b),
            component<Blue,  uint16_t>(b),
            0
        };

        v_b = (o_a * v_a + n_a * v_b) >> 8;

        b = Color { v_b[0], v_b[1], v_b[2] };
    }
    else {
        uint32_t o_a = component<Alpha, uint32_t>(a);
        if (o_a == 0xFF) return a;

        uint32_t n_a = 0xFF - o_a;
        uint32_t o_b = h;

        o_a <<= 8;
        n_a *= o_b;
        uint32_t h = o_a + n_a;

        if ((h >> 8) > 0)
        {
            typedef uint32_t v4ui32 __attribute__((vector_size(16)));

            v4ui32 v_a {
                component<Red,   uint32_t>(a),
                component<Green, uint32_t>(a),
                component<Blue,  uint32_t>(a),
                0
            };

            v4ui32 v_b {
                component<Red,   uint32_t>(b),
                component<Green, uint32_t>(b),
                component<Blue,  uint32_t>(b),
                0
            };

            v_b = (o_a * v_a + n_a * v_b) / h;

            b = Color { v_b[0], v_b[1], v_b[2], h >> 8 };
        }
        else {
            b.w_ = 0;
        }
    }

    return b;
}

String str(Color c)
{
    return Format()
        << "#"
        << hex(Color::redComponent  (c), 2)
        << hex(Color::greenComponent(c), 2)
        << hex(Color::blueComponent (c), 2)
        << hex(Color::alphaComponent(c), 2);
}

} // namespace cc
