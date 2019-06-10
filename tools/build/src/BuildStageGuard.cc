/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Process>
#include "BuildStage.h"
#include "BuildStageGuard.h"

namespace ccbuild {

BuildStageGuard::BuildStageGuard(BuildStage *stage):
    stage_{stage}
{
    for (const String &command: stage_->preCommands())
        Process::execute(command);
}

BuildStageGuard::~BuildStageGuard()
{
    if (stage_->success()) {
        for (const String &command: stage_->postCommands())
            Process::execute(command);
    }
}

} // namespace ccbuild
