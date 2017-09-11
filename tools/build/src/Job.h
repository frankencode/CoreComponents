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

namespace ccbuild {

using namespace cc;

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

    virtual bool run();

private:
    friend class JobServer;

    Job(String command):
        command_(command),
        status_(-1)
    {}

    String command_;

    int status_;
    String outputText_;
};

typedef Channel< Ref<Job> > JobChannel;

} // namespace ccbuild
