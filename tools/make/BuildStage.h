/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_BUILDSTAGE_H
#define FLUXMAKE_BUILDSTAGE_H

namespace fluxmake {

using namespace flux;

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

} // namespace fluxmake

#endif // FLUXMAKE_BUILDSTAGE_H
