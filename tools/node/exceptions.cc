/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "exceptions.h"

namespace ccnode {

String TlsError::message() const { return "TLS error"; }

} // namespace ccnode
