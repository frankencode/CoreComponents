/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanMaster>
#include <cc/can/NetworkCommand>

namespace cc {
namespace can {

Ref<CanMaster> CanMaster::create(const CanMedia &media)
{
    return new CanMaster{media};
}

CanMaster::CanMaster(const CanMedia &media):
    media_{media}
{}

void CanMaster::issueNetworkCommand(NetworkCommand::Specifier type, int who)
{
    media_->writeFrame(NetworkCommand::createFrame(type, who));
}

}} // namespace cc::can
