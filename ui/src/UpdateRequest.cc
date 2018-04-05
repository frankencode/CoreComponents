/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/UpdateRequest>

namespace cc {
namespace ui {

UpdateRequest::UpdateRequest(UpdateReason reason, View *view):
    reason_(reason),
    view_(view)
{}

}} // namespace cc::ui
