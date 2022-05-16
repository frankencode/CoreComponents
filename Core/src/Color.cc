/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Color>
#include <cc/Map>
#include <cc/Casefree>
#include <cc/str>
#include <algorithm>
#include <limits>
#include <cmath>

namespace cc {

const Color Color::Transparent { 0x00, 0x00, 0x00, 0x00 };
const Color Color::Black       { 0x00, 0x00, 0x00 };
const Color Color::White       { 0xFF, 0xFF, 0xFF };
const Color Color::Red         { 0xFF, 0x00, 0x00 };
const Color Color::Green       { 0x00, 0xFF, 0x00 };
const Color Color::Blue        { 0x00, 0x00, 0xFF };

Color Color::fromHsv(double h, double s, double v)
{
    double r, g, b;
    hsvToRgb(h, s, v, &r, &g, &b);
    return Color{
        uint32_t(round(0xFF * r)),
        uint32_t(round(0xFF * g)),
        uint32_t(round(0xFF * b))
    };
}

Color Color::fromHsl(double h, double s, double l)
{
    double r, g, b;
    hslToRgb(h, s, l, &r, &g, &b);
    return Color{
        uint32_t(round(0xFF * r)),
        uint32_t(round(0xFF * g)),
        uint32_t(round(0xFF * b))
    };
}

void Color::applyOver(Color b)
{
    const uint32_t h = component<AlphaShift, uint32_t>();

    if (h == 0xFF)
    {
        uint16_t o_b = b.component<AlphaShift, uint16_t>();
        if (o_b == 0xFF) {
            *this = b;
            return;
        }

        uint16_t n_b = 0x100 - o_b;

        typedef uint16_t v4ui16 __attribute__((vector_size(8)));

        v4ui16 v_b {
            b.component<RedShift  , uint16_t>(),
            b.component<GreenShift, uint16_t>(),
            b.component<BlueShift , uint16_t>(),
            0
        };

        v4ui16 v_a {
            component<RedShift,   uint16_t>(),
            component<GreenShift, uint16_t>(),
            component<BlueShift,  uint16_t>(),
            0
        };

        v_a = (o_b * v_b + n_b * v_a) >> 8;

        me = compose(v_a[0], v_a[1], v_a[2]);
    }
    else {
        uint32_t o_b = b.component<AlphaShift, uint32_t>();
        if (o_b == 0xFF) {
            *this = b;
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
                b.component<RedShift,   uint32_t>(),
                b.component<GreenShift, uint32_t>(),
                b.component<BlueShift,  uint32_t>(),
                0
            };

            v4ui32 v_a {
                component<RedShift,   uint32_t>(),
                component<GreenShift, uint32_t>(),
                component<BlueShift,  uint32_t>(),
                0
            };

            v_a = (o_b * v_b + n_b * v_a) / h;

            me = compose(v_a[0], v_a[1], v_a[2], h >> 8);
        }
        else {
            me = 0;
        }
    }
}

void Color::mixIn(Color b, int percent)
{
    typedef uint32_t v4ui32 __attribute__((vector_size(16)));

    v4ui32 v_b {
        b.component<RedShift,   uint32_t>(),
        b.component<GreenShift, uint32_t>(),
        b.component<BlueShift,  uint32_t>(),
        b.component<AlphaShift, uint32_t>()
    };

    v4ui32 v_a {
        component<RedShift,   uint32_t>(),
        component<GreenShift, uint32_t>(),
        component<BlueShift,  uint32_t>(),
        component<AlphaShift, uint32_t>()
    };

    v_a = (v_a * (100 - percent) + v_b * percent) / 100;

    me = compose(v_a[0], v_a[1], v_a[2], v_a[3]);
}

String Color::toString() const
{
    List<String> parts;
    parts << "#" << hex(red(), 2) << hex(green(), 2) << hex(blue(), 2);
    if (alpha() != 0xFF) parts << hex(alpha(), 2);
    return parts;
}

Color Color::parse(const char *s)
{
    if (!s[0]) return Color{};

    if (s[0] == '#') {
        struct H {
            static inline int dehex(char ch, Out<bool> ok) {
                if ('0' <= ch && ch <= '9') return ch - '0';
                else if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
                else if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
                ok = false;
                return -1;
            }
        };
        Color c;
        int n = 1;
        while (s[n] && n < 11) ++n;
        if (n == 7) { // #RRGGBB
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int rl = H::dehex(s[2], &ok);
            int gh = H::dehex(s[3], &ok);
            int gl = H::dehex(s[4], &ok);
            int bh = H::dehex(s[5], &ok);
            int bl = H::dehex(s[6], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl
            };
        }
        else if (n == 9) { // #RRGGBBAA
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int rl = H::dehex(s[2], &ok);
            int gh = H::dehex(s[3], &ok);
            int gl = H::dehex(s[4], &ok);
            int bh = H::dehex(s[5], &ok);
            int bl = H::dehex(s[6], &ok);
            int ah = H::dehex(s[7], &ok);
            int al = H::dehex(s[8], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl,
                (ah << 4) | al
            };
        }
        else if (n == 4) { // #RGB
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int gh = H::dehex(s[2], &ok);
            int bh = H::dehex(s[3], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh
            };
        }
        else if (n == 5) { // #RGBA
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int gh = H::dehex(s[2], &ok);
            int bh = H::dehex(s[3], &ok);
            int ah = H::dehex(s[4], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh,
                (ah << 4) | ah
            };
        }
        return Color{};
    }
    Color c;
    lookup(s, &c);
    return c;
}

bool Color::lookup(const String &name, Out<Color> color)
{
    static thread_local Map<Casefree<String>, Color> svgPalette {
        {"aliceblue"           , { 240, 248, 255 }},
        {"antiquewhite"        , { 250, 235, 215 }},
        {"aqua"                , {   0, 255, 255 }},
        {"aquamarine"          , { 127, 255, 212 }},
        {"azure"               , { 240, 255, 255 }},
        {"beige"               , { 245, 245, 220 }},
        {"bisque"              , { 255, 228, 196 }},
        {"black"               , {   0,   0,   0 }},
        {"blanchedalmond"      , { 255, 235, 205 }},
        {"blue"                , {   0,   0, 255 }},
        {"blueviolet"          , { 138,  43, 226 }},
        {"brown"               , { 165,  42,  42 }},
        {"burlywood"           , { 222, 184, 135 }},
        {"cadetblue"           , {  95, 158, 160 }},
        {"chartreuse"          , { 127, 255,   0 }},
        {"chocolate"           , { 210, 105,  30 }},
        {"coral"               , { 255, 127,  80 }},
        {"cornflowerblue"      , { 100, 149, 237 }},
        {"cornsilk"            , { 255, 248, 220 }},
        {"crimson"             , { 220,  20,  60 }},
        {"cyan"                , {   0, 255, 255 }},
        {"darkblue"            , {   0,   0, 139 }},
        {"darkcyan"            , {   0, 139, 139 }},
        {"darkgoldenrod"       , { 184, 134,  11 }},
        {"darkgray"            , { 169, 169, 169 }},
        {"darkgreen"           , {   0, 100,   0 }},
        {"darkgrey"            , { 169, 169, 169 }},
        {"darkkhaki"           , { 189, 183, 107 }},
        {"darkmagenta"         , { 139,   0, 139 }},
        {"darkolivegreen"      , {  85, 107,  47 }},
        {"darkorange"          , { 255, 140,   0 }},
        {"darkorchid"          , { 153,  50, 204 }},
        {"darkred"             , { 139,   0,   0 }},
        {"darksalmon"          , { 233, 150, 122 }},
        {"darkseagreen"        , { 143, 188, 143 }},
        {"darkslateblue"       , {  72,  61, 139 }},
        {"darkslategray"       , {  47,  79,  79 }},
        {"darkslategrey"       , {  47,  79,  79 }},
        {"darkturquoise"       , {   0, 206, 209 }},
        {"darkviolet"          , { 148,   0, 211 }},
        {"deeppink"            , { 255,  20, 147 }},
        {"deepskyblue"         , {   0, 191, 255 }},
        {"dimgray"             , { 105, 105, 105 }},
        {"dimgrey"             , { 105, 105, 105 }},
        {"dodgerblue"          , {  30, 144, 255 }},
        {"firebrick"           , { 178,  34,  34 }},
        {"floralwhite"         , { 255, 250, 240 }},
        {"forestgreen"         , {  34, 139,  34 }},
        {"fuchsia"             , { 255,   0, 255 }},
        {"gainsboro"           , { 220, 220, 220 }},
        {"ghostwhite"          , { 248, 248, 255 }},
        {"gold"                , { 255, 215,   0 }},
        {"goldenrod"           , { 218, 165,  32 }},
        {"gray"                , { 128, 128, 128 }},
        {"grey"                , { 128, 128, 128 }},
        {"green"               , {   0, 128,   0 }},
        {"greenyellow"         , { 173, 255,  47 }},
        {"honeydew"            , { 240, 255, 240 }},
        {"hotpink"             , { 255, 105, 180 }},
        {"indianred"           , { 205,  92,  92 }},
        {"indigo"              , {  75,   0, 130 }},
        {"ivory"               , { 255, 255, 240 }},
        {"khaki"               , { 240, 230, 140 }},
        {"lavender"            , { 230, 230, 250 }},
        {"lavenderblush"       , { 255, 240, 245 }},
        {"lawngreen"           , { 124, 252,   0 }},
        {"lemonchiffon"        , { 255, 250, 205 }},
        {"lightblue"           , { 173, 216, 230 }},
        {"lightcoral"          , { 240, 128, 128 }},
        {"lightcyan"           , { 224, 255, 255 }},
        {"lightgoldenrodyellow", { 250, 250, 210 }},
        {"lightgray"           , { 211, 211, 211 }},
        {"lightgreen"          , { 144, 238, 144 }},
        {"lightgrey"           , { 211, 211, 211 }},
        {"lightpink"           , { 255, 182, 193 }},
        {"lightsalmon"         , { 255, 160, 122 }},
        {"lightseagreen"       , {  32, 178, 170 }},
        {"lightskyblue"        , { 135, 206, 250 }},
        {"lightslategray"      , { 119, 136, 153 }},
        {"lightslategrey"      , { 119, 136, 153 }},
        {"lightsteelblue"      , { 176, 196, 222 }},
        {"lightyellow"         , { 255, 255, 224 }},
        {"lime"                , {   0, 255,   0 }},
        {"limegreen"           , {  50, 205,  50 }},
        {"linen"               , { 250, 240, 230 }},
        {"magenta"             , { 255,   0, 255 }},
        {"maroon"              , { 128,   0,   0 }},
        {"mediumaquamarine"    , { 102, 205, 170 }},
        {"mediumblue"          , {   0,   0, 205 }},
        {"mediumorchid"        , { 186,  85, 211 }},
        {"mediumpurple"        , { 147, 112, 219 }},
        {"mediumseagreen"      , {  60, 179, 113 }},
        {"mediumslateblue"     , { 123, 104, 238 }},
        {"mediumspringgreen"   , {   0, 250, 154 }},
        {"mediumturquoise"     , {  72, 209, 204 }},
        {"mediumvioletred"     , { 199,  21, 133 }},
        {"midnightblue"        , {  25,  25, 112 }},
        {"mintcream"           , { 245, 255, 250 }},
        {"mistyrose"           , { 255, 228, 225 }},
        {"moccasin"            , { 255, 228, 181 }},
        {"navajowhite"         , { 255, 222, 173 }},
        {"navy"                , {   0,   0, 128 }},
        {"oldlace"             , { 253, 245, 230 }},
        {"olive"               , { 128, 128,   0 }},
        {"olivedrab"           , { 107, 142,  35 }},
        {"orange"              , { 255, 165,   0 }},
        {"orangered"           , { 255,  69,   0 }},
        {"orchid"              , { 218, 112, 214 }},
        {"palegoldenrod"       , { 238, 232, 170 }},
        {"palegreen"           , { 152, 251, 152 }},
        {"paleturquoise"       , { 175, 238, 238 }},
        {"palevioletred"       , { 219, 112, 147 }},
        {"papayawhip"          , { 255, 239, 213 }},
        {"peachpuff"           , { 255, 218, 185 }},
        {"peru"                , { 205, 133,  63 }},
        {"pink"                , { 255, 192, 203 }},
        {"plum"                , { 221, 160, 221 }},
        {"powderblue"          , { 176, 224, 230 }},
        {"purple"              , { 128,   0, 128 }},
        {"red"                 , { 255,   0,   0 }},
        {"rosybrown"           , { 188, 143, 143 }},
        {"royalblue"           , {  65, 105, 225 }},
        {"saddlebrown"         , { 139,  69,  19 }},
        {"salmon"              , { 250, 128, 114 }},
        {"sandybrown"          , { 244, 164,  96 }},
        {"seagreen"            , {  46, 139,  87 }},
        {"seashell"            , { 255, 245, 238 }},
        {"sienna"              , { 160,  82,  45 }},
        {"silver"              , { 192, 192, 192 }},
        {"skyblue"             , { 135, 206, 235 }},
        {"slateblue"           , { 106,  90, 205 }},
        {"slategray"           , { 112, 128, 144 }},
        {"slategrey"           , { 112, 128, 144 }},
        {"snow"                , { 255, 250, 250 }},
        {"springgreen"         , {   0, 255, 127 }},
        {"steelblue"           , {  70, 130, 180 }},
        {"tan"                 , { 210, 180, 140 }},
        {"teal"                , {   0, 128, 128 }},
        {"thistle"             , { 216, 191, 216 }},
        {"tomato"              , { 255,  99,  71 }},
        {"turquoise"           , {  64, 224, 208 }},
        {"violet"              , { 238, 130, 238 }},
        {"wheat"               , { 245, 222, 179 }},
        {"white"               , { 255, 255, 255 }},
        {"whitesmoke"          , { 245, 245, 245 }},
        {"yellow"              , { 255, 255,   0 }},
        {"yellowgreen"         , { 154, 205,  50 }}
    };

    return svgPalette.lookup(name, &color);
}

void rgbToHsv(double r, double g, double b, Out<double> h, Out<double> s, Out<double> v)
{
    double min, max; {
        auto pair = std::minmax<double>({r, g, b});
        min = pair.first;
        max = pair.second;
    }

    v = max;
    s = (max != 0.) ? ((max - min) / max) : 0.;
    if (s == 0.) {
        h = 0.;
        return;
    }

    double delta = max - min;
    if (r == max)
        h = (g - b) / delta;
    else if (g == max)
        h = 2. + (b - r) / delta;
    else /*if (b == max) */
        h = 4. + (r - g) / delta;
    h = h * 60.;
    if (h < 0.) h = h + 360.;
}

void hsvToRgb(double h, double s, double v, Out<double> r, Out<double> g, Out<double> b)
{
    if (s == 0.) {
        r = v;
        g = v;
        b = v;
        return;
    }

    double f, p, q, t;
    int i;

    if (h == 360.) h = 0.;
    h /= 60.;
    i = floor(h);
    f = h - i;
    p = v * (1. - s);
    q = v * (1. - s * f);
    t = v * (1. - s * (1. - f));

    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    };
}

void rgbToHsl(double r, double g, double b, Out<double> h, Out<double> s, Out<double> l)
{
    double min, max; {
        auto pair = std::minmax<double>({r, g, b});
        min = pair.first;
        max = pair.second;
    }

    l = (min + max) / 2.;
    if (min == max) {
        s = 0.;
        h = 0.;
        return;
    }

    double delta = max - min;
    s = (l <= 0.5) ? (delta / (min + max)) : (delta / (2. - (min + max)));
    if (r == max)
        h = (g - b) / delta;
    else if (g == max)
        h = 2. + (b - r) / delta;
    else if (b == max)
        h = 4. + (r - g) / delta;
    h = h * 60.;
    if (h < 0.) h = h + 360.;
}

void hslToRgb(double h, double s, double l, Out<double> r, Out<double> g, Out<double> b)
{
    double c = (1. - abs(2. * l - 1.)) * s;
    double q = h / 60.;
    double x = c * (1. - abs(fmod(q, 2.) - 1.));

    double r1, g1, b1;
    if (q <= 1)      { r1 = c; g1 = x; b1 = 0; }
    else if (q <= 2) { r1 = x; g1 = c; b1 = 0; }
    else if (q <= 3) { r1 = 0; g1 = c; b1 = x; }
    else if (q <= 4) { r1 = 0; g1 = x; b1 = c; }
    else if (q <= 5) { r1 = x; g1 = 0; b1 = c; }
    else             { r1 = c; g1 = 0; b1 = x; }

    double m = l - c / 2.;
    r = r1 + m;
    g = g1 + m;
    b = b1 + m;
}

const TypeInfo VariantType<Color>::info
{
    .typeName = "Color",
    .str = [](const void *bytes) {
        return VariantType<Color>::retrieve(bytes).toString();
    },
    .cleanup = [](void *bytes){},
    .assign = [](void *dst, const void *src) {
        new(dst)Color{*static_cast<const Color *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Color>::retrieve(a) == VariantType<Color>::retrieve(b);
    }
};

} // namespace cc
