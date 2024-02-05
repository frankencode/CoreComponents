/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyMessageSource>
#include <cc/build/CodyBlockSource>

namespace cc::build {

struct CodyMessageSource::State final: public Object::State
{
    explicit State(const Stream &stream):
        blockSource_{stream}
    {}

    bool read(Out<CodyMessage> message)
    {
        while (block_.count() == 0) {
            if (!blockSource_.read(&block_)) {
                return false;
            }
        }

        message = CodyMessage::read(block_.first());
        block_.popFront();
        return true;
    }

    CodyBlockSource blockSource_;
    List<String> block_;
};

CodyMessageSource::CodyMessageSource(const Stream &stream):
    Object{new State{stream}}
{}

bool CodyMessageSource::read(Out<CodyMessage> message)
{
    return me().read(&message);
}

CodyMessageSource::State &CodyMessageSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
