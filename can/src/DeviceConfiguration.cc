/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/DeviceConfiguration>
#include <cc/Format>

namespace cc {
namespace can {

Ref<DeviceConfiguration> DeviceConfiguration::create()
{
    return new DeviceConfiguration;
}

Ref<DeviceConfiguration> DeviceConfiguration::parse(const String &text)
{
    Ref<DeviceConfiguration> config = new DeviceConfiguration;
    config->IniSyntax::parse(text);
    return config;
}

DeviceConfiguration::DeviceConfiguration():
    comissioning_{new Comissioning}
{}

String DeviceConfiguration::toString() const
{
    return Format()
        << DeviceDescription::toString() << nl
        << comissioning_->toString();
}

bool DeviceConfiguration::equals(const DeviceConfiguration *other) const
{
    return
        comissioning_->equals(other->comissioning_) &&
        DeviceDescription::equals(other);
}

void DeviceConfiguration::enterSection(const String &sectionName)
{
    if (sectionName->match(comissioning_->sectionName())) setCurrentSection(comissioning_);
    else DeviceDescription::enterSection(sectionName);
}

DeviceConfiguration::Comissioning::Comissioning():
    Section{"DeviceComissioning"}
{}

void DeviceConfiguration::Comissioning::establish(const String &key, const String &value)
{
    if (key->match("NodeID")) nodeId_ = value->toNumber<uint8_t>();
    else if (key->match("NodeName")) nodeName_ = value;
    else if (key->match("Baudrate")) baudRate_ = value->toNumber<uint16_t>() * 1000;
    else if (key->match("NetNumber")) networkNumber_ = value->toNumber<uint32_t>();
    else if (key->match("NetworkName")) networkName_ = value;
    else if (key->match("CANopenManager")) manager_ = value->toNumber<int>();
    else if (key->match("LSS_SerialNumber")) serialNumber_ = value->toNumber<uint32_t>();
}

String DeviceConfiguration::Comissioning::toString() const
{
    Format f;
    f << "[" << sectionName() << "]" << nl;
    f << "NodeID=" << nodeId_ << nl;
    if (nodeName_ != "") f << "NodeName=" << nodeName_ << nl;
    if (baudRate_ != 0) f << "Baudrate=" << baudRate_ / 1000 << nl;
    f << "NetNumber=" << networkNumber_ << nl;
    f << "NetworkName=" << networkName_ << nl;
    if (manager_) f << "CANopenManager=1" << nl;
    f << "LSS_SerialNumber=" << serialNumber_ << nl;
    return f;
}

bool DeviceConfiguration::Comissioning::equals(const Comissioning *other) const
{
    return
        nodeId_ == other->nodeId_ &&
        nodeName_ == other->nodeName_ &&
        baudRate_ == other->baudRate_ &&
        networkNumber_ == other->networkNumber_ &&
        networkName_ == other->networkName_ &&
        manager_ == other->manager_ &&
        serialNumber_ == other->serialNumber_;
}

}} // namespace cc::can
