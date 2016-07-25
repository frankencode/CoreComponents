/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/File>
#include <cc/stdio>

namespace cc {

template<int fd>
class StdIo: public Object, public Singleton< StdIo<fd> >
{
public:
    StdIo():
        stream_(SystemStream::create(fd))
    {
        stream_->setScatterLimit(-1);
    }
    Ref<SystemStream> stream_;
};

SystemStream *stdIn() { return StdIo<StdInFd>::instance()->stream_; }
SystemStream *stdOut() { return StdIo<StdOutFd>::instance()->stream_; }
SystemStream *stdErr() { return StdIo<StdErrFd>::instance()->stream_; }

Format fout(String pattern) { return Format(pattern, stdOut()); }
Format ferr(String pattern) { return Format(pattern, stdErr()); }

} // namespace cc
