/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/DictionaryManager>
#include <cc/isobus/CsvMessageDictionary>
#include <cc/isobus/CsvSubsystemDictionary>
#include <cc/isobus/CsvVendorDictionary>
#include <cc/isobus/CsvIndustryDictionary>
#include <cc/CsvSource>
#include <cc/Process>
#include <cc/File>

namespace cc::j1939 {

struct DictionaryManager::State final: public Singleton::State
{
    State():
        dataPath_{Process::env("HOME") / ".local" / "share" / "CoreComponentsCAN"},
        messageTypesCsvPath_{dataPath_ / "messages.csv"},
        subsystemsCsvPath_{dataPath_ / "subsystems.csv"},
        vendorsCsvPath_{dataPath_ / "vendors.csv"},
        industriesCsvPath_{dataPath_ / "industries.csv"}
    {
        messageTypeDictionary_ = isobus::CsvMessageDictionary{File{messageTypesCsvPath_}.map()};
        subsystemDictionary_ = isobus::CsvSubsystemDictionary{File{subsystemsCsvPath_}.map()};
        vendorDictionary_ = isobus::CsvVendorDictionary{File{vendorsCsvPath_}.map()};
        industryDictionary_ = isobus::CsvIndustryDictionary{File{industriesCsvPath_}.map()};
    }

    String dataPath_;
    String messageTypesCsvPath_;
    String subsystemsCsvPath_;
    String vendorsCsvPath_;
    String industriesCsvPath_;
    MessageDictionary messageTypeDictionary_;
    SubsystemDictionary subsystemDictionary_;
    VendorDictionary vendorDictionary_;
    IndustryDictionary industryDictionary_;
};

DictionaryManager::DictionaryManager():
    Singleton{instance<State>()}
{}

String DictionaryManager::dataPath() const
{
    return me().dataPath_;
}

const MessageDictionary &DictionaryManager::messageTypeDictionary() const
{
    return me().messageTypeDictionary_;
}

const SubsystemDictionary &DictionaryManager::subsystemDictionary() const
{
    return me().subsystemDictionary_;
}

const VendorDictionary &DictionaryManager::vendorDictionary() const
{
    return me().vendorDictionary_;
}

const IndustryDictionary &DictionaryManager::industryDictionary() const
{
    return me().industryDictionary_;
}

const DictionaryManager::State &DictionaryManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::j1939
