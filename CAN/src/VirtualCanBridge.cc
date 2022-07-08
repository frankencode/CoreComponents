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
    State(const CanMedia &mediaA, const CanMedia &mediaB):
        mediaA_{mediaA},
        mediaB_{mediaB},
        aToB_{[this]{
            for (CanFrame frame; mediaA_.readFrame(&frame);) {
                mediaB_.writeFrame(frame);
            }
        }},
        bToA_{[this]{
            for (CanFrame frame; mediaB_.readFrame(&frame);) {
                mediaA_.writeFrame(frame);
            }
        }}
    {
        aToB_.start();
        bToA_.start();
    }

    ~State()
    {
        aToB_.wait();
        bToA_.wait();
    }

    CanMedia mediaA_, mediaB_;
    Thread aToB_, bToA_;
};

VirtualCanBridge::VirtualCanBridge(const CanMedia &mediaA, const CanMedia &mediaB):
    Object{new State{mediaA, mediaB}}
{}

} // namespace cc
