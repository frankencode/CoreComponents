/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Singleton>
#include <cc/File>

namespace cc {

template<int fd>
class StdIo: public Object, public Singleton< StdIo<fd> >
{
public:
    StdIo():
        stream_{fd}
    {
        stream_->setScatterLimit(-1);
    }

    SystemStream stream_;
};

SystemStream &stdIn () { return StdIo<StandardInputFd >::instance()->stream_; }
SystemStream &stdOut() { return StdIo<StandardOutputFd>::instance()->stream_; }
SystemStream &stdErr() { return StdIo<StandardErrorFd >::instance()->stream_; }

} // namespace cc
