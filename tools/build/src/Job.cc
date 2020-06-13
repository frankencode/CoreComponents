/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "Job.h"
#include <cc/Spawn>
#include <cc/Mutex>
#include <cc/stdio>

namespace ccbuild {

void Job::registerDerivative(Job *derivative)
{
    derivatives_->pushBack(derivative);
    ++derivative->countDown_;
}

bool Job::run()
{
    try {
        Spawn sub{command_};
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
