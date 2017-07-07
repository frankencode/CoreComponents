/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio> // DEBUG
#include <cc/ColorNames>
#include <cc/Format>
#include <cc/Color>

namespace cc {

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
    return Color();
}

Color Color::blend(Color a, Color b)
{
    uint32_t o_a = Color::alpha(a);
    if (o_a == 0xFF) return a;

    o_a += (o_a == 0xFF);
    uint32_t n_a = 0x100 - o_a;

    uint32_t o_b = Color::alpha(b);
    if (o_b == 0xFF) {
        for (int i = FirstChannel; i <= LastChannel; ++i)
            b.bytes_[i] = (o_a * a.bytes_[i] + n_a * b.bytes_[i]) >> 8;
    }
    else {
        o_b += (o_b >= 0xFF);
        o_a <<= 8;
        n_a *= o_b;
        uint32_t h = o_a + n_a;
        if ((Color::alpha(b) = h >> 8) > 0) {
            for (int i = FirstChannel; i <= LastChannel; ++i)
                b.bytes_[i] = (o_a * a.bytes_[i] + n_a * b.bytes_[i]) / h;
        }
        else {
            b.word_ = 0;
        }
    }

    return b;
}

String str(Color c)
{
    return Format()
        << "#"
        << hex(Color::red(c), 2)
        << hex(Color::green(c), 2)
        << hex(Color::blue(c), 2)
        << hex(Color::alpha(c), 2);
}

} // namespace cc
