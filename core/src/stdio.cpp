/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/File>
#include <flux/stdio>

namespace flux {

template<int fd>
class StdIo: public Object, public Singleton< StdIo<fd> >
{
public:
    StdIo():
        stream_(SystemStream::create(fd))
    {}
    Ref<SystemStream> stream_;
};

SystemStream *stdIn() { return StdIo<File::StandardInput>::instance()->stream_; }
SystemStream *stdOut() { return StdIo<File::StandardOutput>::instance()->stream_; }
SystemStream *stdErr() { return StdIo<File::StandardError>::instance()->stream_; }


} // namespace flux
