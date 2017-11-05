/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Property>

namespace cc {

thread_local void *PropertyEvaluationContext::activeInstance_ = 0;

} // namespace cc
