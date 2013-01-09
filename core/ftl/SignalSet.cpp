/*
 * SignalSet.cpp -- set of signals
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "SignalSet.hpp"

namespace ftl
{

SignalSet::SignalSet(int preset) { if (preset == Empty) sigemptyset(this); else sigfillset(this); }
void SignalSet::insert(int signal) { sigaddset(this, signal); }
void SignalSet::remove(int signal) { sigdelset(this, signal); }
bool SignalSet::contains(int signal) const { return sigismember(this, signal); }

} // namespace ftl
