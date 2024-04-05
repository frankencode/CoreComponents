/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanReplayMedia>
#include <cc/LineSource>

namespace cc {

struct CanReplayMedia::State final: public CanMedia::State
{
    explicit State(const String &text):
        source_{text}
    {}

    bool waitFrame(int timeout) override
    {
        return true;
    }

    bool read(Out<CanFrame> frame) override
    {
        bool gotOne = false;
        for (String line; source_.read(&line);) {
            uint64_t deltaTime = 0;
            if (CanFrame::read(line, &frame, &deltaTime)) {
                gotOne = true;
                time_ += deltaTime / 1000.;
                break;
            }
        }
        return gotOne;
    }

    void write(const CanFrame &frame) override
    {}

    double time() const override
    {
        return time_;
    }

    LineSource source_;
    double time_ { 0 };
};

CanReplayMedia::CanReplayMedia(const String &text):
    CanMedia{new State{text}}
{}

} // namespace cc
