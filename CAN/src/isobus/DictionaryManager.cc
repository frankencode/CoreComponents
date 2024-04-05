/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/DictionaryManager>
#include <cc/j1939/DictionaryManager>
#include <cc/File>

namespace cc::isobus {

struct DictionaryManager::State final: public Singleton::State
{
    State():
        dataPath_{other_.dataPath()},
        processDataDictionaryPath_{dataPath_ / "process_data.txt"},
        processDataDictionary_{File{processDataDictionaryPath_}.map()},
        messageTypeDictionary_{other_.messageTypeDictionary()},
        subsystemDictionary_{other_.subsystemDictionary()},
        vendorDictionary_{other_.vendorDictionary()},
        industryDictionary_{other_.industryDictionary()}
    {}

    j1939::DictionaryManager other_;
    String dataPath_;
    String processDataDictionaryPath_;
    isobus::pd::Dictionary processDataDictionary_;
    j1939::MessageDictionary messageTypeDictionary_;
    j1939::SubsystemDictionary subsystemDictionary_;
    j1939::VendorDictionary vendorDictionary_;
    j1939::IndustryDictionary industryDictionary_;
};

DictionaryManager::DictionaryManager():
    Singleton{instance<State>()}
{}

String DictionaryManager::dataPath() const
{
    return me().dataPath_;
}

const isobus::pd::Dictionary &DictionaryManager::processDataDictionary() const
{
    return me().processDataDictionary_;
}

const j1939::MessageDictionary &DictionaryManager::messageTypeDictionary() const
{
    return me().messageTypeDictionary_;
}

const j1939::SubsystemDictionary &DictionaryManager::subsystemDictionary() const
{
    return me().subsystemDictionary_;
}

const j1939::VendorDictionary &DictionaryManager::vendorDictionary() const
{
    return me().vendorDictionary_;
}

const j1939::IndustryDictionary &DictionaryManager::industryDictionary() const
{
    return me().industryDictionary_;
}

const DictionaryManager::State &DictionaryManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::isobus
