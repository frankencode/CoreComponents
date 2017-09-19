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

namespace cc {
namespace ui {

class Box: public Object
{
public:
    inline bool isValid() const { return w_ * h_ > 0; }

    inline static Ref<Box> create() {
        return new Box;
    }

    inline static Ref<Box> create(double x, double y, double w, double h) {
        return new Box(x, y, w, h);
    }

    inline static Ref<Box> copy(const Box *other) {
        return new Box(other);
    }

    inline Ref<Box> copy() const {
        return new Box(this);
    }

    inline double x() const { return x_; }
    inline double y() const { return y_; }
    inline double w() const { return w_; }
    inline double h() const { return h_; }

    inline void moveTo(double x, double y) { x_ = x; y_ = y; }
    inline void resize(double w, double h) { w_ = w; h_ = h; }

    void inset(double margin);

    String toString() const;

protected:
    Box():
        x_(0), y_(0), w_(0), h_(0)
    {}

    Box(double x, double y, double w, double h):
        x_(x), y_(y), w_(w), h_(h)
    {}

    Box(const Box *other):
        x_(other->x_),
        y_(other->y_),
        w_(other->w_),
        h_(other->h_)
    {}

    double x_, y_, w_, h_;
};

}} // namespace cc::ui

