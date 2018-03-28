/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/ui/SdlApplication>
#include <cc/ui/SdlTimeMaster>

namespace cc {
namespace ui {

SdlTimeMaster *SdlTimeMaster::instance()
{
    return Singleton<SdlTimeMaster>::instance();
}

void SdlTimeMaster::triggerTimer(const Timer *timer)
{
    SdlApplication::instance()->triggerTimer(timer);
}

}} // namespace cc::ui
