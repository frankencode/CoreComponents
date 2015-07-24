/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_CWDGUARD_H
#define FLUXMAKE_CWDGUARD_H

#include "BuildShell.h"

namespace fluxmake
{

using namespace flux;

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

} // namespace fluxmake

#endif // FLUXMAKE_CWDGUARD_H
