/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace ccbuild {

class BuildStage;

class BuildStageGuard
{
public:
    BuildStageGuard(BuildStage *stage);
    ~BuildStageGuard();

private:
    BuildStage *stage_;
};

} // namespace ccbuild
