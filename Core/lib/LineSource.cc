/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LineSource>

namespace cc {

struct LineSource::State: public Object::State
{
    State(const Stream &stream, const String &buffer):
        stream_{stream},
        buffer_{buffer},
        eoi_{false},
        bufferOffset_{0},
        i0_{0},
        i_{0},
        n_{(stream) ? 0 : buffer.count()}
    {}

    bool read(Out<String> line)
    {
        if (eoi_) {
            line << String{};
            return false;
        }

        List<String> backlog;

        while (true) {
            if (i_ < n_) {
                i0_ = i_;
                i_ = findEol(buffer_, n_, i_);
                if (i_ < n_) {
                    if (backlog.count() > 0) {
                        backlog.append(buffer_.copy(i0_, i_));
                        line << backlog;
                    }
                    else {
                        line << buffer_.copy(i0_, i_);
                    }
                    i_ = skipEol(buffer_, n_, i_);
                    return true;
                }
                backlog.append(buffer_.copy(i0_, i_));
            }

            if (!stream_) break;

            bufferOffset_ += n_;
            n_ = stream_.read(&buffer_);
            if (n_ == 0) break;
            i_ = 0;
        }

        eoi_ = true;
        if (backlog.count() > 0) {
            line << backlog;
            return true;
        }
        line << String{};
        return false;
    }

    static long findEol(const Bytes &buffer, long n, long i)
    {
        for (; i < n; ++i) {
            char ch = buffer.at(i);
            if (ch == '\n' || ch == '\r' || ch == '\0')
                break;
        }
        return i;
    }

    static long skipEol(const Bytes &buffer, long n, long i)
    {
        if (i < n) if (buffer.at(i) == '\r') ++i;
        if (i < n) if (buffer.at(i) == '\n') ++i;
        if (i < n) if (buffer.at(i) == '\0') ++i;
        return i;
    }

    Stream stream_;
    String buffer_;
    bool eoi_;
    long bufferOffset_;
    long i0_, i_, n_;
};

LineSource::LineSource(const String &buffer):
    Object{new State{Stream{}, buffer}}
{}

LineSource::LineSource(const Stream &stream):
    Object{new State{stream, String::allocate(0x1000)}}
{}

LineSource::LineSource(const Stream &stream, const String &buffer):
    Object{new State{stream, buffer}}
{}

bool LineSource::read(Out<String> line)
{
    return me().read(line);
}

LineSource::State &LineSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc
