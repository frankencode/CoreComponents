/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Color>

namespace cc {

template<class Char, class Value> class PrefixTree;

template<class SubClass> class Singleton;

/** \brief Lookup table for SVG standard color names
  */
class ColorNames: public Object
{
public:
    static const ColorNames *instance();

    bool lookup(const char *name, Color *color) const;

private:
    friend class Singleton<ColorNames>;

    ColorNames();

    typedef PrefixTree<char, Color> ColorByName;
    Ref<ColorByName> colorByName_;
};

} // namespace cc
