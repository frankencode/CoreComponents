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
#include <cc/Variant>
#include <cc/colors>
#include <cc/Color>

namespace cc {

const Color Color::Transparent { 0x00, 0x00, 0x00, 0x00 };
const Color Color::Black       { 0x00, 0x00, 0x00 };
const Color Color::White       { 0xFF, 0xFF, 0xFF };
const Color Color::Red         { 0xFF, 0x00, 0x00 };
const Color Color::Green       { 0x00, 0xFF, 0x00 };
const Color Color::Blue        { 0x00, 0x00, 0xFF };

Color::Color(const Variant &v)
{
    *this = Variant::cast<Color>(v);
}

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

void Color::Instance::applyOver(Color b)
{
    const uint32_t h = component<AlphaShift, uint32_t>();

    if (h == 0xFF)
    {
        uint16_t o_b = b->component<AlphaShift, uint16_t>();
        if (o_b == 0xFF) {
            w_ = b->w_;
            return;
        }

        uint16_t n_b = 0x100 - o_b;

        typedef uint16_t v4ui16 __attribute__((vector_size(8)));

        v4ui16 v_b {
            b->component<RedShift  , uint16_t>(),
            b->component<GreenShift, uint16_t>(),
            b->component<BlueShift , uint16_t>(),
            0
        };

        v4ui16 v_a {
            component<RedShift,   uint16_t>(),
            component<GreenShift, uint16_t>(),
            component<BlueShift,  uint16_t>(),
            0
        };

        v_a = (o_b * v_b + n_b * v_a) >> 8;

        w_ = compose(v_a[0], v_a[1], v_a[2]);
    }
    else {
        uint32_t o_b = b->component<AlphaShift, uint32_t>();
        if (o_b == 0xFF) {
            w_ = b->w_;
            return;
        }

        uint32_t n_b = 0xFF - o_b;
        uint32_t o_a = h;

        o_b <<= 8;
        n_b *= o_a;
        uint32_t h = o_b + n_b;

        if ((h >> 8) > 0)
        {
            typedef uint32_t v4ui32 __attribute__((vector_size(16)));

            v4ui32 v_b {
                b->component<RedShift,   uint32_t>(),
                b->component<GreenShift, uint32_t>(),
                b->component<BlueShift,  uint32_t>(),
                0
            };

            v4ui32 v_a {
                component<RedShift,   uint32_t>(),
                component<GreenShift, uint32_t>(),
                component<BlueShift,  uint32_t>(),
                0
            };

            v_a = (o_b * v_b + n_b * v_a) / h;

            w_ = compose(v_a[0], v_a[1], v_a[2], h >> 8);
        }
        else {
            w_ = 0;
        }
    }
}

String Color::Instance::toString() const
{
    return Format()
        << "#"
        << hex(red(), 2)
        << hex(green(), 2)
        << hex(blue(), 2)
        << hex(alpha(), 2);
}

} // namespace cc
