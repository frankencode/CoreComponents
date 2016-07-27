/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Event>

namespace cc {

/** \class Property Property.h cc/Property
  */
template<class T>
class Property
{
public:
    Property() {}
    Property(const T &b): value_(b) {}

    inline Property &operator=(const T &b) {
        if (b != value_) {
            value_ = b;
            if (valueChanged_)
                valueChanged_->notify(value_);
        }
        return *this;
    }

    inline operator const T&() const { return value_; }

    inline Event<T> *valueChanged() {
        if (!valueChanged_) valueChanged_ = Event<T>::create();
        return valueChanged_;
    }

    inline Property *operator->() { return this; }
    inline const Property *operator->() const { return this; }

private:
    Ref< Event<T> > valueChanged_;
    T value_;
};

} // namespace cc
