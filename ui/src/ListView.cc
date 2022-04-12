/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ListView>

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
            size(size() + Size{0, child.height()});
        }

        void removeChild(View child) override
        {
            View::State::removeChild(child);
        }

        Property<double> grayExtent;
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
