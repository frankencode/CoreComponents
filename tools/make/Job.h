/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_JOB_H
#define FLUXMAKE_JOB_H

#include <flux/String>
#include <flux/Channel>

namespace fluxmake {

using namespace flux;

class JobServer;

class Job: public Object
{
public:
    inline static Ref<Job> create(String command) {
        return new Job(command);
    }

    inline String command() const { return command_; }

    inline int status() const { return status_; }
    inline String outputText() const { return outputText_; }

private:
    friend class JobServer;

    Job(String command)
        : command_(command),
          status_(-1)
    {}

    String command_;

    int status_;
    String outputText_;
};

typedef Channel< Ref<Job> > JobChannel;

} // namespace fluxmake

#endif // FLUXMAKE_JOB_H
