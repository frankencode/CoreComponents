/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Set>
#include <cc/PropertyBinding>

namespace cc {

thread_local PropertyBinding *PropertyBinding::activeInstance_ = 0;

PropertyBinding::PropertyBinding(bool dirty):
    dirty_(dirty)
{}

void PropertyBinding::preAccess() const
{
    if (dirty_) {
        const_cast<PropertyBinding *>(this)->dirty_ = false;
        const_cast<PropertyBinding *>(this)->cascade();
    }

    if ((activeInstance_) && (activeInstance_ != this)) {
        if (activeInstance_->dependencies()->insert(const_cast<PropertyBinding *>(this)))
            subscribers()->insert(activeInstance_);
    }
}

PropertyBinding::Association *PropertyBinding::dependencies() const
{
    if (!dependencies_) dependencies_ = Association::create();
    return dependencies_;
}

PropertyBinding::Association *PropertyBinding::subscribers() const
{
    if (!subscribers_) subscribers_ = Association::create();
    return subscribers_;
}

bool PropertyBinding::hasConsumers() const
{
    return
        (subscribers_ && subscribers_->count() > 0) ||
        changed->hasListeners();
}

void PropertyBinding::clearDependencies()
{
    if (!dependencies_) return;

    for (PropertyBinding *other: dependencies_)
        other->subscribers_->remove(this);
    dependencies_ = nullptr;
}

void PropertyBinding::clearSubscribers()
{
    if (!subscribers_) return;

    for (PropertyBinding *other: subscribers_)
        other->dependencies_->remove(this);
    subscribers_ = nullptr;
}

void PropertyBinding::emit()
{
    if (subscribers_) {
        for (PropertyBinding *other: subscribers_) {
            if (other->hasConsumers()) {
                other->dirty_ = false;
                other->evaluate();
            }
            else {
                other->dirty_ = true;
                other->evaluateLater();
            }
        }
    }
    changed->emit();
}

void PropertyBinding::cascade()
{
    class Activator {
    public:
        Activator(PropertyBinding *instance): savedActiveInstance_(PropertyBinding::activeInstance_) {
            PropertyBinding::activeInstance_ = instance;
        }
        ~Activator() {
            PropertyBinding::activeInstance_ = savedActiveInstance_;
        }
    private:
        PropertyBinding *savedActiveInstance_;
    };

    Activator activator(this);
    evaluate();
}

void PropertyBinding::disband()
{
    changed->disband();
    clearDependencies();
    if (subscribers_) {
        while (subscribers_->count() > 0) {
            PropertyBinding *other = subscribers_->at(0);
            other->disband();
        }
        subscribers_ = nullptr;
    }
}

} // namespace cc
