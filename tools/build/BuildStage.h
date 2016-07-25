/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace ccbuild {

using namespace cc;

class BuildPlan;
class BuildShell;
class ToolChain;
class JobScheduler;

class BuildStage
{
public:
    inline bool complete() const { return complete_; }
    inline bool success() const { return success_; }
    inline int status() const { return status_; }

protected:
    BuildStage(BuildPlan *plan);

    inline BuildPlan *plan() const { return plan_; }
    BuildShell *shell() const;
    ToolChain *toolChain() const;
    Ref<JobScheduler> createScheduler() const;

    bool outOfScope() const;

    BuildPlan *plan_;
    bool complete_, success_;
    int status_;
};

} // namespace ccbuild
