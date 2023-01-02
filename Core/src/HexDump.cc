/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HexDump>
#include <cc/Format>
#include <cc/IoStream>
#include <cc/CaptureSink>

namespace cc {

struct HexDump::State final: public Stream::State
{
    State(const Stream &sink):
        sink_{sink},
        buffer_{Bytes::allocate(16)}
    {
        if (!sink_) sink_ = IoStream::output();
    }

    ~State()
    {
        flush();
    }

    void write(const Bytes &data, long fill = -1) override
    {
        if (fill < 0) fill = data.count();
        for (long i = 0; i < fill; ++i) {
            writeByte(data.at(i));
        }
    }

    void writeByte(uint8_t ch)
    {
        if (i_ == buffer_.count()) flush();
        buffer_.at(i_) = ch;
        ++i_;
    }

    void flush()
    {
        Format f{sink_};
        f << indent_ << hex(offset_, 8) << "  ";
        for (long j = 0; j < buffer_.count(); ++j) {
            if (j < i_) {
                uint8_t &ch = buffer_.at(j);
                f << hex(ch, 2);
                if (ch < 0x20 || 0x7F <= ch) ch = '.';
            }
            else f << "  ";
            f << (((j + 1) % 8 == 0) ? "  " : " ");
        }
        f << "|" << buffer_.select(0, i_) << "|" << nl;
        offset_ += i_;
        i_ = 0;
    }

    Stream sink_;
    Bytes buffer_;
    long i_ { 0 };
    long long offset_ { 0 };
    String indent_;
};

HexDump::HexDump(const Stream &sink):
    Stream{new State{sink}}
{}

HexDump &HexDump::indent(const String &prefix)
{
    me().indent_ = prefix;
    return *this;
}

HexDump::State &HexDump::me()
{
    return Object::me.as<State>();
}

String hexdump(const Bytes &data)
{
    CaptureSink capture;
    HexDump{capture}.write(data);
    return capture.collect();
}

} // namespace cc
