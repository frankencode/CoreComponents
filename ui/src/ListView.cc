/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ListView>
#include <cc/Layout>

namespace cc::ui {

class ListView::Carrier final: public View
{
public:

private:
    struct State final: public View::State
    {
        void insertChild(View child) override
        {
            child.pos({0, height()});
            View::State::insertChild(child);
            layout_.pushBack(child, child.height());
            size(Size{width(), layout_.extent()});
        }

        void removeChild(View child) override
        {
            layout_.remove(child);
            View::State::removeChild(child);
            size(Size{width(), layout_.extent()});
        }

        Property<double> grayExtent;
        cc::Layout<View, double> layout_;
    };
};

struct ListView::State final: public Flickable::State
{
    State()
        /*TODO*/
    {
        /*TODO*/
    }

};

} // namespace cc::ui
