/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PropertyBinding>
#include <cc/SetInstance>

namespace cc {

String PropertyBindingLoop::message() const
{
    return "Property binding loop detected";
}

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

PropertyBinding::PropertyBinding(bool dirty):
    dirty_{dirty}
{}

void PropertyBinding::preAccess() const
{
    if (dirty_) {
        const_cast<PropertyBinding *>(this)->dirty_ = false;
        const_cast<PropertyBinding *>(this)->cascade();
    }

    PropertyBinding *activeBinding = PropertyActivator::activeBinding();
    if (activeBinding && (activeBinding != this)) {
        if (activeBinding->dependencies_->insert(const_cast<PropertyBinding *>(this)))
            subscribers_->insert(activeBinding);
    }
}

bool PropertyBinding::hasConsumers() const
{
    return subscribers_->count() > 0 || changed->hasListeners();
}

void PropertyBinding::clearDependencies()
{
    for (PropertyBinding *other: dependencies_)
        other->subscribers_->remove(this);
    dependencies_->deplete();
}

void PropertyBinding::clearSubscribers()
{
    for (PropertyBinding *other: subscribers_)
        other->dependencies_->remove(this);
    subscribers_->deplete();
}

void PropertyBinding::emit()
{
    Association others = subscribers_;
    for (PropertyBinding *other: others) {
        if (other->hasConsumers()) {
            other->dirty_ = false;
            other->cascade();
        }
        else
            other->dirty_ = true;
    }

    PropertyActivator activator{nullptr}; // FIXME
    changed->emit();
}

void PropertyBinding::cascade()
{
    PropertyActivator activator{this};
    evaluate();
}

void PropertyBinding::disband()
{
    changed->disband();
    clearDependencies();
    while (subscribers_->count() > 0) {
        PropertyBinding *other = subscribers_->at(0);
        other->disband();
    }
    subscribers_->deplete();
}

} // namespace cc
