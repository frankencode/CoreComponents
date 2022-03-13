/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PropertyBinding>

namespace cc {

class PropertyActivator
{
public:
    PropertyActivator(PropertyBinding *binding):
        parent_{head_},
        binding_{binding}
    {
        if (binding_) {
            ++binding_->cascadeDepth_;
            if (binding_->cascadeDepth_ > 3) throw PropertyBindingLoop{};
        }
        head_ = this;
    }

    ~PropertyActivator()
    {
        if (binding_) --binding_->cascadeDepth_;
        head_ = parent_;
    }

    static PropertyBinding *activeBinding() { return head_ ? head_->binding_ : nullptr; }

    PropertyActivator *operator->() { return this; }

private:
    static thread_local PropertyActivator *head_;
    PropertyActivator *parent_ { nullptr };
    PropertyBinding *binding_ { nullptr };
};

thread_local PropertyActivator *PropertyActivator::head_ { nullptr };

PropertyBinding::PropertyBinding(void *owner, bool dirty):
    owner_{owner},
    dirty_{dirty}
{}

void PropertyBinding::preAccess() const
{
    if (dirty_) const_cast<PropertyBinding *>(this)->cascade();

    PropertyBinding *activeBinding = PropertyActivator::activeBinding();
    if (activeBinding && (activeBinding != this)) {
        if (activeBinding->dependencies_.insert(Handle<PropertyBinding>::alias(const_cast<PropertyBinding *>(this))))
            subscribers_.insert(Handle<PropertyBinding>::alias(activeBinding));
    }
}

bool PropertyBinding::hasConsumers() const
{
    return subscribers_.count() > 0 || changed.hasListeners();
}

void PropertyBinding::clearDependencies()
{
    for (auto &other: dependencies_)
        other().subscribers_.remove(this);
    dependencies_.deplete();
}

void PropertyBinding::clearSubscribers()
{
    for (auto &other: subscribers_)
        other().dependencies_.remove(this);
    subscribers_.deplete();
}

void PropertyBinding::emit()
{
    Association others = subscribers_;
    for (auto &other: others) {
        if (other().hasConsumers())
            other.mutate().cascade();
        else
            other.mutate().dirty_ = true;
    }

    PropertyActivator activator{nullptr};
    changed.emit();
}

void PropertyBinding::cascade()
{
    PropertyActivator activator{this};
    evaluate();
}

void PropertyBinding::disband()
{
    changed.disband();
    clearDependencies();
    while (subscribers_.count() > 0)
        subscribers_.at(0).mutate().disband();
    subscribers_.deplete();
}

} // namespace cc
