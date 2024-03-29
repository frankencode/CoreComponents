/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#ifndef NDEBUG
#define CCBUILD_CODY_DEBUG
#endif

#include <cc/build/CodyTransport>
#include <cc/build/CodyBlockSource>
#include <cc/input>
#ifdef CCBUILD_CODY_DEBUG
#include <cc/Format>
#include <cc/stdio>
#endif

namespace cc::build {

struct CodyTransport::State final: public Object::State
{
    explicit State(const Stream &stream):
        stream_{stream},
        blockSource_{stream}
    {}

    bool read(Out<CodyMessage> message)
    {
        while (inBlock_.count() == 0) {
            if (!blockSource_.read(&inBlock_)) {
                return false;
            }
            outCount_ = inBlock_.size();
            #ifdef CCBUILD_CODY_DEBUG
            String prefix = Format{"(%%) >> "} << connectionNumber_;
            ferr() << (prefix + inBlock_.join("\n" + prefix)) << nl;
            #endif
        }

        message = CodyMessage::read(inBlock_.first());
        inBlock_.popFront();
        return true;
    }

    void write(const String &message)
    {
        if (outCount_ == 0) {
            stream_.write(message + "\n");
            return;
        }

        --outCount_;
        outBlock_.append(message);
        outBlock_.append((outCount_ == 0) ? "\n" : " ;\n");
        if (outCount_ == 0) {
            #ifdef CCBUILD_CODY_DEBUG
            String prefix = Format{"(%%) << "} << connectionNumber_;
            ferr() << indent(outBlock_.join(), prefix) << nl;
            #endif
            stream_.write(outBlock_.join());
            outBlock_.deplete();
        }
    }

    static int getNextConnectionNumber()
    {
        static int n = 0;
        return ++n;
    }

    #ifdef CCBUILD_CODY_DEBUG
    int connectionNumber_ { getNextConnectionNumber() };
    #endif
    Stream stream_;
    CodyBlockSource blockSource_;
    List<String> inBlock_;
    List<String> outBlock_;
    long outCount_ { 0 };
};

CodyTransport::CodyTransport(const Stream &stream):
    Object{new State{stream}}
{}

bool CodyTransport::read(Out<CodyMessage> message)
{
    return me().read(&message);
}

void CodyTransport::write(const String &message)
{
    me().write(message);
}

CodyTransport::State &CodyTransport::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
