/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Process>
#include <cc/Mutex>
#include "Job.h"

namespace ccbuild {

void Job::registerDerivative(Job *derivative)
{
    if (!derivatives_) derivatives_ = Job::Derivatives::create();
    derivatives_->pushBack(derivative);
    ++derivative->countDown_;
}

bool Job::run()
{
    try {
        Ref<Process> sub = Process::open(command_);
        outputText_ = sub->output()->readAll();
        status_ = sub->wait();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
        status_ = -1;
    }

    return status_ == 0;
}

} // namespace ccbuild
