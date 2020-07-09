/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HexDump>
#include <cc/CaptureSink>
#include <cc/stdio>

namespace cc {

HexDump::Instance::Instance(const Stream &sink):
    sink_{sink},
    offset_{0},
    buffer_{String::allocate(16)},
    i_{0}
{
    if (!sink_) sink_ = stdOut();
}

HexDump::Instance::~Instance()
{
    flush();
}

void HexDump::Instance::write(const CharArray *data)
{
    for (int i = 0; i < data->count(); ++i)
        writeByte(data->byteAt(i));
}

void HexDump::Instance::writeByte(uint8_t ch)
{
    if (i_ == buffer_->count()) flush();
    mutate(buffer_)->byteAt(i_) = ch;
    ++i_;
}

void HexDump::Instance::flush()
{
    Format f{sink_};
    f << hex(offset_, 8) << "  ";
    for (int j = 0; j < buffer_->count(); ++j) {
        if (j < i_) {
            uint8_t &ch = mutate(buffer_)->byteAt(j);
            f << hex(ch, 2);
            if (ch < 0x20 || 0x7F <= ch) ch = '.';
        }
        else f << "  ";
        f << (((j + 1) % 8 == 0) ? "  " : " ");
    }
    f << "|" << buffer_->select(0, i_) << "|" << nl;
    offset_ += i_;
    i_ = 0;
}

String hexDump(const CharArray *data)
{
    CaptureSink capture;
    HexDump{capture}->write(data);
    return capture->collect();
}

} // namespace cc
