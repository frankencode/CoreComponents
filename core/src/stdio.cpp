/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadLocalSingleton>
#include <flux/stdio>

namespace flux {

class StdIo: public Object, public ThreadLocalSingleton<StdIo>
{
public:
    inline File *stdIn() {
        if (!stdIn_) stdIn_ = File::open(File::StandardInput, File::ReadOnly);
        return stdIn_;
    }

    inline File *stdOut() {
        if (!stdOut_) stdOut_ = File::open(File::StandardOutput, File::WriteOnly);
        return stdOut_;
    }

    inline File *stdErr() {
        if (!stdErr_) stdErr_ = File::open(File::StandardError, File::WriteOnly);
        return stdErr_;
    }

    inline LineSource *lineInput() {
        if (!lineInput_) lineInput_ = LineSource::open(stdIn());
        return lineInput_;
    }

    Ref<File> stdIn_;
    Ref<File> stdOut_;
    Ref<File> stdErr_;
    Ref<LineSource> lineInput_;
};

File *stdIn() { return StdIo::instance()->stdIn(); }
File *stdOut() { return StdIo::instance()->stdOut(); }
File *stdErr() { return StdIo::instance()->stdErr(); }

LineSource *lineInput() { return StdIo::instance()->lineInput(); }

} // namespace flux
