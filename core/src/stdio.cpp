/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadLocalSingleton>
#include <flux/File>
#include <flux/stdio>

namespace flux {

class StdIo: public Object, public ThreadLocalSingleton<StdIo>
{
public:
    inline SystemStream *stdIn() {
        if (!stdIn_) stdIn_ = File::open(File::StandardInput, File::ReadOnly);
        return stdIn_;
    }

    inline SystemStream *stdOut() {
        if (!stdOut_) stdOut_ = File::open(File::StandardOutput, File::WriteOnly);
        return stdOut_;
    }

    inline SystemStream *stdErr() {
        if (!stdErr_) stdErr_ = File::open(File::StandardError, File::WriteOnly);
        return stdErr_;
    }

    Ref<SystemStream> stdIn_;
    Ref<SystemStream> stdOut_;
    Ref<SystemStream> stdErr_;
};

SystemStream *stdIn() { return StdIo::instance()->stdIn(); }
SystemStream *stdOut() { return StdIo::instance()->stdOut(); }
SystemStream *stdErr() { return StdIo::instance()->stdErr(); }

void setStdInt(SystemStream *newIn)
{
    if (::dup2(newIn->fd(), File::StandardInput) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    StdIo::instance()->stdIn_ = newIn;
}

void setStdOut(SystemStream *newOut)
{
    if (::dup2(newOut->fd(), File::StandardOutput) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    StdIo::instance()->stdOut_ = newOut;
}

void setStdErr(SystemStream *newErr)
{
    if (::dup2(newErr->fd(), File::StandardError) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    StdIo::instance()->stdErr_ = newErr;
}

} // namespace flux
