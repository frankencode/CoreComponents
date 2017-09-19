/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Color>

namespace cc {
namespace ui {

class FontProvider;

class Font;
typedef List< Ref<Font> > FontList; // FIXME: whatfore?

class Font: public Object
{
public:
    /** width to height ratio
      */
    enum Stretch {
        UltraCondensed   =  500,
        ExtraCondensed   =  625,
        Condensed        =  750,
        SemiCondensed    =  875,
        Unstretched      = 1000,
        SemiExpanded     = 1125,
        Expanded         = 1250,
        ExtraExpanded    = 1500,
        UltraExpanded    = 2000
    };

    /** visual thickness
      */
    enum Weight {
        Thin       = 100,
        ExtraLight = 200,
        Light      = 300,
        Normal     = 400,
        Medium     = 500,
        SemiBold   = 600,
        Bold       = 700,
        ExtraBold  = 800,
        Black      = 900
    };

    static Ref<Font> select(String family = "");

    inline Ref<Font> copy() const { return new Font(this); }

    bool equals(const Font *other) const;

    inline int id() const { return id_; }
    inline String family() const { return family_; }

    inline double size() const { return size_; }
    inline void setSize(double newSize) { size_ = newSize; }

    inline bool italic() const { return italic_; }
    inline void setItalic(bool on) { italic_ = on; }

    inline Stretch stretch() const { return stretch_; }
    inline void setStretch(Stretch newStretch) { stretch_ = newStretch; }

    inline Weight weight() const { return weight_; }
    inline void setWeight(Weight newWeight) { weight_ = newWeight; }

    inline Color color() const { return color_; }
    inline void setColor(Color newColor) { color_ = newColor; }

    inline bool hinting() const { return hinting_; }
    inline void setHinting(bool on) { hinting_ = on; }

    static String stretchName(Stretch stretch);
    static String weightName(Weight weight);

protected:
    friend class FontProvider;

    Font(int id, String family);
    Font(const Font *other);

    int id_;
    String family_;
    double size_;
    bool italic_;
    Stretch stretch_;
    Weight weight_;
    Color color_;
    bool hinting_;
};

}} // namespace cc::ui

