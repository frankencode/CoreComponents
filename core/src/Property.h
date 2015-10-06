/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PROPERTY_H
#define FLUX_PROPERTY_H

#include <flux/Event>

namespace flux {

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

} // namespace flux

#endif // FLUX_PROPERTY_H
