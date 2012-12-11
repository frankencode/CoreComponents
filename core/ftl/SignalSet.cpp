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

SignalSet::SignalSet() { clear(); }
void SignalSet::clear() { sigemptyset(&rawSet_); }
void SignalSet::fill() { sigfillset(&rawSet_); }
void SignalSet::add(int signal) { sigaddset(&rawSet_, signal); }
void SignalSet::del(int signal) { sigdelset(&rawSet_, signal); }
bool SignalSet::contains(int signal) const { return sigismember(&rawSet_, signal); }
sigset_t *SignalSet::rawSet() { return &rawSet_; }

} // namespace ftl
