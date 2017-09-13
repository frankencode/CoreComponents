/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Channel>
#include <cc/Queue>

namespace ccbuild {

using namespace cc;

class JobServer;
class JobScheduler;

class Job: public Object
{
public:
    inline static Ref<Job> create(String command) {
        return new Job(command);
    }

    inline String command() const { return command_; }

    inline int status() const { return status_; }
    inline String outputText() const { return outputText_; }

    virtual bool run();

private:
    friend class JobServer;
    friend class JobScheduler;

    Job(String command):
        command_(command),
        status_(-1),
        countDown_(0)
    {}

    String command_;

    int status_;
    String outputText_;

    int countDown_;

    typedef Queue< Ref<Job> > Derivatives;
    Ref<Derivatives> derivatives_;
};

typedef Channel< Ref<Job> > JobChannel;

} // namespace ccbuild
