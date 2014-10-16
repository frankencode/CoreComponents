/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "SignalSet"

namespace flux
{

SignalSet::SignalSet(int preset) { if (preset == Empty) sigemptyset(&rawSet_); else sigfillset(&rawSet_); }
void SignalSet::insert(int signal) { sigaddset(&rawSet_, signal); }
void SignalSet::remove(int signal) { sigdelset(&rawSet_, signal); }
bool SignalSet::contains(int signal) const { return sigismember(&rawSet_, signal); }

} // namespace flux
