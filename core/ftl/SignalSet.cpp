 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "SignalSet.hpp"

namespace ftl
{

SignalSet::SignalSet(int preset) { if (preset == Empty) sigemptyset(this); else sigfillset(this); }
void SignalSet::insert(int signal) { sigaddset(this, signal); }
void SignalSet::remove(int signal) { sigdelset(this, signal); }
bool SignalSet::contains(int signal) const { return sigismember(this, signal); }

} // namespace ftl
