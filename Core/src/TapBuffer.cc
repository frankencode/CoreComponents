/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TapBuffer>
#include <cc/Thread>
#include <cc/str>
#include <cc/DEBUG>

namespace cc {

struct TapBuffer::State: public LineBuffer::State
{
    State(const Stream &stream, const String &prefix):
        LineBuffer::State{stream, prefix}
    {}

    void write(const Bytes &buffer, long fill) override
    {
        List<Bytes> parts;
        long i = 0, i0 = 0;
        if (fill < 0) fill = buffer.count();
        for (long n = fill; i < n; ++i) {
            uint8_t ch = buffer.byteAt(i);
            if (ch <= 31 || 127 <= ch) {
                if (i0 < i) parts.append(buffer.copy(i0, i));
                i0 = i + 1;
                if (ch == 0x08) parts.append(String{"\\b", 2});
                else if (ch == 0x09) parts.append(String{"\\t", 2});
                else if (ch == 0x0A) parts.append(String{"\\n\n", 3});
                else if (ch == 0x0D) parts.append(String{"\\r", 2});
                else if (ch == 0x0C) parts.append(String{"\\f", 2});
                else {
                    String s { "\\xXX" };
                    const char *hex = "0123456789ABCDEF";
                    s.at(s.count() - 2) = hex[ch / 16];
                    s.at(s.count() - 1) = hex[ch % 16];
                    parts.append(s);
                    if (ch == 0) parts.append(String{"\n", 1});
                }
            }
        }
        if (parts.count() > 0) {
            if (i0 < i) parts.append(buffer.copy(i0, i));
            LineBuffer::State::write(parts.join());
        }
        else
            LineBuffer::State::write(buffer, fill);
    }

    String prefix() const override
    {
        return "(" + inum(Thread::id(), 62) + ") " + LineBuffer::State::prefix();
    }
};

TapBuffer::TapBuffer(const Stream &stream, const String &prefix):
    LineBuffer{new State{stream, prefix}}
{}

} // namespace cc
