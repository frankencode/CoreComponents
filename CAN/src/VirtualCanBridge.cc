/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/VirtualCanBridge>
#include <cc/CanMedia>
#include <cc/Thread>

namespace cc {

struct VirtualCanBridge::State: public Object::State
{
    State(const CanMedia &busA, const CanMedia &busB):
        busA_{busA},
        busB_{busB},
        aToB_{[this]{ for (CanFrame frame: busA_) busB_.write(frame); }},
        bToA_{[this]{ for (CanFrame frame: busB_) busA_.write(frame); }}
    {
        aToB_.start();
        bToA_.start();
    }

    ~State()
    {
        aToB_.wait();
        bToA_.wait();
    }

    CanMedia busA_, busB_;
    Thread aToB_, bToA_;
};

VirtualCanBridge::VirtualCanBridge(const CanMedia &busA, const CanMedia &busB):
    Object{new State{busA, busB}}
{}

} // namespace cc
