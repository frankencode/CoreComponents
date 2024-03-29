/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LineBuffer>

namespace cc {

LineBuffer::State::State(const Stream &sink, const String &prefix):
    sink_{sink},
    prefix_{prefix}
{}

long LineBuffer::State::read(Out<Bytes> buffer, long maxFill)
{
    return (sink_) ? sink_.read(buffer, maxFill) : 0;
}

void LineBuffer::State::write(const Bytes &buffer, long fill)
{
    long i = 0, n = (fill < 0 || buffer.count() < fill) ? buffer.count() : fill;

    while (i < n) {
        long i0 = i;
        if (buffer.find('\n', &i)) {
            ++i;
            backlog_.pushBack(buffer.copy(i0, i));
            i0 = i;
            flush();
        }
        else {
            backlog_.pushBack(buffer.copy(i0, n));
        }
    }
}

String LineBuffer::State::prefix() const
{
    return prefix_;
}

long LineBuffer::State::flush()
{
    String h = prefix();
    if (backlog_.count() == 0) return 0;
    if (h != "") backlog_.pushFront(h);
    Bytes line{backlog_};
    writeLine(line);
    backlog_.deplete();
    return line.count();
}

void LineBuffer::State::writeLine(const Bytes &line)
{
    if (sink_) sink_.write(line);
}

LineBuffer::LineBuffer(const Stream &sink, const String &prefix):
    Stream{new LineBuffer::State{sink, prefix}}
{}

LineBuffer::LineBuffer(State *newState):
    Stream{newState}
{}

long LineBuffer::flush()
{
    return me().flush();
}

} // namespace cc
