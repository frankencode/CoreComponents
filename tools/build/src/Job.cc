/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/SubProcess>
#include "Job.h"

namespace ccbuild {

bool Job::run()
{
    try {
        Ref<SubProcess> sub = SubProcess::open(command_);
        outputText_ = sub->readAll();
        status_ = sub->wait();
    }
    catch (Exception &ex) {
        ferr() << ex << nl;
        status_ = -1;
    }

    return status_ == 0;
}

} // namespace ccbuild
