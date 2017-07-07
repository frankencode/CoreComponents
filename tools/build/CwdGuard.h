/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildShell.h"

namespace ccbuild
{

using namespace cc;

class CwdGuard {
public:
    CwdGuard(String dirPath, BuildShell *shell):
        shell_(shell),
        cwdSaved_(Process::cwd())
    {
        shell->cd(dirPath);
    }
    ~CwdGuard() { shell_->cd(cwdSaved_); }
private:
    BuildShell *shell_; // FIXME: use Ref<BuildShell>
    String cwdSaved_;
};

} // namespace ccbuild

