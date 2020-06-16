/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildStageGuard.h"
#include <cc/String>

namespace ccbuild {

using namespace cc;

class BuildPlan;
class BuildShell;
class ToolChain;
class JobScheduler;

class BuildStage
{
public:
    bool complete() const { return complete_; }
    bool success() const { return success_; }
    int status() const { return status_; }

    StringList preCommands() const { return preCommands_; }
    StringList postCommands() const { return postCommands_; }

protected:
    BuildStage(BuildPlan *plan);

    BuildPlan *plan() const { return plan_; }
    const BuildShell *shell() const;
    ToolChain *toolChain() const;
    Ref<JobScheduler> createScheduler() const;

    bool outOfScope() const;

    BuildPlan *plan_;
    bool complete_, success_;
    int status_;
    StringList preCommands_;
    StringList postCommands_;
};

} // namespace ccbuild
