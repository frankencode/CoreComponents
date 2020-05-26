/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>
#include <cc/Channel>
#include <cc/Queue>

namespace ccbuild {

using namespace cc;

class JobServer;
class JobScheduler;

class Job: public Object
{
public:
    static Ref<Job> create(const string &command) {
        return new Job{command};
    }

    string command() const { return command_; }

    int status() const { return status_; }
    string outputText() const { return outputText_; }

    void registerDerivative(Job *derivative);

    virtual bool run();
    virtual bool finish() { return true; }

protected:
    friend class JobServer;
    friend class JobScheduler;

    Job(const string &command):
        command_{command},
        status_{-1},
        countDown_{0}
    {}

private:
    string command_;

    int status_;
    string outputText_;

    int countDown_;

    typedef Queue< Ref<Job> > Derivatives;
    Ref<Derivatives> derivatives_;
};

typedef Channel< Ref<Job> > JobChannel;

} // namespace ccbuild
