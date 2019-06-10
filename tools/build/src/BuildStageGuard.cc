/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SubProcess>
#include "BuildStage.h"
#include "BuildStageGuard.h"

namespace ccbuild {

BuildStageGuard::BuildStageGuard(BuildStage *stage):
    stage_{stage}
{
    for (String command: stage_->preCommands()) {
        SubProcess::stage()
            ->setCommand(command)
            ->start()
            ->wait();
    }
}

BuildStageGuard::~BuildStageGuard()
{
    if (stage_->success()) {
        for (String command: stage_->postCommands()) {
            SubProcess::stage()
                ->setCommand(command)
                ->start()
                ->wait();
        }
    }
}

} // namespace ccbuild
