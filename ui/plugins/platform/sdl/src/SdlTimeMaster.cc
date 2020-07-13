/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SdlTimeMaster>
#include <cc/ui/SdlApplicationInstance>
#include <cc/Singleton>

namespace cc {
namespace ui {

SdlTimeMaster *SdlTimeMaster::instance()
{
    return Singleton<SdlTimeMaster>::instance();
}

void SdlTimeMaster::triggerTimer(const Timer::Instance *timer)
{
    SdlApplicationInstance::instance()->triggerTimer(timer);
}

}} // namespace cc::ui
