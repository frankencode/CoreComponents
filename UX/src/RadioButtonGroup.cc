/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/RadioButtonGroup>
#include <cc/RadioButtonControl>

namespace cc {

struct RadioButtonGroup::State final: public Entity::State
{
    void join(RadioButtonControl newMember)
    {
        newMember.onSelected([this,newMember]{
            for (RadioButtonControl member: members_) {
                if (member != newMember) {
                    member.value(false);
                }
            }
        });

        members_.append(newMember);
    }

    List<RadioButtonControl> members_;
};

RadioButtonGroup::RadioButtonGroup():
    Entity{onDemand<State>}
{}

void RadioButtonGroup::join(const RadioButtonControl &newMember)
{
    me().join(newMember);
}

RadioButtonGroup::State &RadioButtonGroup::me()
{
    return get<State>();
}

} // namespace cc
