/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "Job.h"

namespace ccbuild {

class BuildPlan;

class LinkJob: public Job
{
public:
    static Ref<LinkJob> create(const BuildPlan *plan) { return new LinkJob(plan); }

     virtual bool run() override;

private:
    LinkJob(const BuildPlan *plan);

    Ref<const BuildPlan> plan_;
};

} // namespace ccbuild
