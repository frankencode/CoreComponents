/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineBuffer>

namespace cc {

LineBuffer::Instance::Instance(const Stream &sink, const String &prefix):
    sink_{sink},
    prefix_{prefix}
{}

void LineBuffer::Instance::writeLine(const CharArray *data)
{
    if (sink_) sink_->write(data);
}

String LineBuffer::Instance::prefix() const
{
    return prefix_;
}

int LineBuffer::Instance::read(CharArray *data)
{
    return (sink_) ? sink_->read(data) : 0;
}

void LineBuffer::Instance::write(const CharArray *data)
{
    int i = 0, n = data->count();
    if (n == 0) return;
    while (i < n) {
        int i0 = i;
        if (data->find('\n', &i)) {
            ++i;
            backlog_->pushBack(data->copy(i0, i));
            i0 = i;
            flush();
        }
        else {
            backlog_->pushBack(data->copy(i0, n));
        }
    }
}

void LineBuffer::Instance::write(const StringList &parts)
{
    for (const String &s: parts)
        write(s);
}

int LineBuffer::Instance::flush()
{
    String h = prefix();
    if (backlog_->count() == 0) return 0;
    if (h != "") backlog_->pushFront(h);
    String line = backlog_->join();
    writeLine(line);
    backlog_ = StringList{};
    return line->count();
}

} // namespace cc
