/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyBlockSource>
#include <cc/LineSource>

namespace cc::build {

struct CodyBlockSource::State final: public Object::State
{
    explicit State(const Stream &stream):
        lineSource_{stream}
    {}

    bool read(Out<List<String>> block)
    {
        bool gotSome = false;
        String line;
        do {
            gotSome = lineSource_.read(&line);
            if (!gotSome) break;
            block->append(line);
        } while (line.endsWith(';'));
        return gotSome;
    }

    LineSource lineSource_;
};

CodyBlockSource::CodyBlockSource(const Stream &stream):
    Object{new State{stream}}
{}

bool CodyBlockSource::read(Out<List<String>> block)
{
    return me().read(&block);
}

CodyBlockSource::State &CodyBlockSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
