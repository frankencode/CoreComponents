/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/String>
#include <cc/List>

namespace cc {
namespace ui {

enum Placement {
    PlaceAuto    = 0,
    PlaceLeft    = 1,
    PlaceRight   = 2,
    PlaceHCenter = 4,
    PlaceTop     = 8,
    PlaceBottom  = 16,
    PlaceVCenter = 32,
    PlaceTopLeft      = PlaceTop|PlaceLeft,
    PlaceTopCenter    = PlaceTop|PlaceHCenter,
    PlaceTopRight     = PlaceTop|PlaceRight,
    PlaceCenterLeft   = PlaceVCenter|PlaceLeft,
    PlaceCenter       = PlaceVCenter|PlaceHCenter,
    PlaceCenterRight  = PlaceVCenter|PlaceRight,
    PlaceBottomLeft   = PlaceBottom|PlaceLeft,
    PlaceBottomCenter = PlaceBottom|PlaceHCenter,
    PlaceBottomRight  = PlaceBottom|PlaceRight
};

class Rect;
typedef List< Ref<Rect> > RectList;

class Rect: public Object
{
public:
    inline bool isValid() { return w_ * h_ > 0; }

    inline static Ref<Rect> create() {
        return new Rect;
    }

    inline static Ref<Rect> create(int x, int y, int w, int h) {
        return new Rect(x, y, w, h);
    }

    inline static Ref<Rect> copy(const Rect *other) { // FIXME: obsolete
        return new Rect(other);
    }

    inline Ref<Rect> copy() const { return new Rect(this); }

    inline int x() const { return x_; }
    inline int y() const { return y_; }
    inline int w() const { return w_; }
    inline int h() const { return h_; }

    virtual void moveTo(int x, int y) { x_ = x; y_ = y; }
    virtual void resize(int w, int h) { w_ = w; h_ = h; }

    void inset(int margin);

    String toString() const;

protected:
    Rect():
        x_(0), y_(0), w_(0), h_(0)
    {}

    Rect(int x, int y, int w, int h):
        x_(x), y_(y), w_(w), h_(h)
    {}

    Rect(const Rect *other):
        x_(other->x_),
        y_(other->y_),
        w_(other->w_),
        h_(other->h_)
    {}

    int x_, y_, w_, h_;
};

}} // namespace cc::ui

