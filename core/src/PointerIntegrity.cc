/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PointerIntegrity>
#include <cc/SpinLock>
#include <cc/Map>
#include <cc/Set>

namespace cc {

struct PointerIntegrity::State
{
    using PointerSetByTarget = Map<void *, Set<void **> >;

    void addPointer(void *target, void **pointer)
    {
        Guard<SpinLock> guard{lock_};

        Locator locator;
        if (!pointerSetByTarget_.find(target, &locator)) {
            pointerSetByTarget_.insert(target, Set<void **>{}, &locator);
        }
        pointerSetByTarget_.valueAt(locator).insert(pointer);
    }

    void dropPointer(void *target, void **pointer)
    {
        Guard<SpinLock> guard{lock_};
        Locator locator;
        if (pointerSetByTarget_.find(target, &locator)) {
            Set<void **> &set = pointerSetByTarget_.valueAt(locator);
            if (set.count() == 1)
                pointerSetByTarget_.removeAt(locator);
            else
                set.remove(pointer);
        }
    }

    void onEndOfLife(void *target)
    {
        Guard<SpinLock> guard{lock_};
        Locator locator;
        if (pointerSetByTarget_.find(target, &locator)) {
            const Set<void **> &set = pointerSetByTarget_.valueAt(locator);
            for (void **p: set) {
                *p = nullptr;
            }
            pointerSetByTarget_.removeAt(locator);
        }
    }

    SpinLock lock_;
    PointerSetByTarget pointerSetByTarget_;
};

void PointerIntegrity::addPointer(void *&pointer)
{
    me().addPointer(pointer, &pointer);
}

void PointerIntegrity::dropPointer(void *&pointer)
{
    me().dropPointer(pointer, &pointer);
}

void PointerIntegrity::onEndOfLife(void *target)
{
    me().onEndOfLife(target);
}

PointerIntegrity::State &PointerIntegrity::slowMe()
{
    static State instance;
    return instance;
}

PointerIntegrity::State &PointerIntegrity::me()
{
    thread_local State &instance = slowMe();
    return instance;
}

} // namespace cc
