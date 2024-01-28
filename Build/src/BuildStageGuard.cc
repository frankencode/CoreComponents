/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildStageGuard>
#include <cc/build/BuildStage>
#include <cc/Process>

namespace cc::build {

BuildStageGuard::BuildStageGuard(const BuildStage *stage):
    stage_{*stage}
{
    for (const String &command: stage_.preCommands()) {
        Process::exec(command);
    }
}

BuildStageGuard::~BuildStageGuard()
{
    if (stage_.success()) {
        for (const String &command: stage_.postCommands()) {
            Process::exec(command);
        }
    }
}

} // namespace cc::build
