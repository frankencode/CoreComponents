/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/DeviceDescription>
#include <cc/LineSource>
#include <cc/Format>
#include <cc/str>
#include <cc/Set>
#include <cc/Map>
#include <cc/Date>
#include <cc/assert>

namespace cc {
namespace can {

Ref<DeviceDescription> DeviceDescription::create()
{
    return new DeviceDescription;
}

Ref<DeviceDescription> DeviceDescription::parse(const String &text)
{
    Ref<DeviceDescription> d = new DeviceDescription;
    d->IniSyntax::parse(text);
    return d;
}

DeviceDescription::DeviceDescription():
    fileInfo_{new FileInfo},
    deviceInfo_{new DeviceInfo},
    dummyUsage_{new DummyUsage},
    comments_{new Comments},
    mandatoryObjects_{new MandatoryObjects},
    optionalObjects_{new OptionalObjects},
    manufacturerObjects_{new ManufacturerObjects},
    currentSection_{nullptr}
{}

String DeviceDescription::toString() const
{
    auto parts = StringList::create();
    parts
        << fileInfo()->toString()
        << deviceInfo()->toString()
        << dummyUsage()->toString()
        << mandatoryObjects()->toString()
        << optionalObjects()->toString()
        << manufacturerObjects()->toString();

    if (comments()->lineCount() > 0) parts << comments()->toString();

    return parts->join("\n");
}

bool DeviceDescription::equals(const DeviceDescription *other) const
{
    return
        fileInfo_->equals(other->fileInfo_) &&
        deviceInfo_->equals(other->deviceInfo_) &&
        dummyUsage_->equals(other->dummyUsage_) &&
        comments_->equals(other->comments_) &&
        mandatoryObjects_->equals(other->mandatoryObjects_) &&
        optionalObjects_->equals(other->optionalObjects_) &&
        manufacturerObjects_->equals(other->manufacturerObjects_);
}

void DeviceDescription::enterSection(const String &sectionName)
{
    if (sectionName->match(fileInfo_->sectionName()))
        currentSection_ = fileInfo_;
    else if (sectionName->match(deviceInfo_->sectionName()))
        currentSection_ = deviceInfo_;
    else if (sectionName->match(dummyUsage_->sectionName()))
        currentSection_ = dummyUsage_;
    else if (sectionName->match(comments_->sectionName()))
        currentSection_ = comments_;
    else if (sectionName->match(mandatoryObjects_->sectionName()))
        currentSection_ = mandatoryObjects_;
    else if (sectionName->match(optionalObjects_->sectionName()))
        currentSection_ = optionalObjects_;
    else if (sectionName->match(manufacturerObjects_->sectionName()))
        currentSection_ = manufacturerObjects_;
    else if (currentSection_)
        currentSection_->enterSection(sectionName);
    else
        currentSection_ = nullptr;
}

void DeviceDescription::establish(const String &key, const String &value)
{
    if (currentSection_) currentSection_->establish(key, value);
}

void DeviceDescription::handleError(int offset, const String &line)
{}

DeviceDescription::FileInfo::FileInfo():
    Section{"FileInfo"}
{}

double DeviceDescription::FileInfo::creationTime() const
{
    return makeTime(creationDate_, creationDayTime_);
}

void DeviceDescription::FileInfo::setCreationTime(double time)
{
    auto date = Date::breakdown(time);
    creationDate_ = makeCalendarDate(date);
    creationDayTime_ = makeDayTime(date);
}

double DeviceDescription::FileInfo::changedTime() const
{
    return makeTime(changedDate_, changedDayTime_);
}

void DeviceDescription::FileInfo::setChangedTime(double time)
{
    auto date = Date::breakdown(time);
    changedDate_ = makeCalendarDate(date);
    changedDayTime_ = makeDayTime(date);
}

String DeviceDescription::FileInfo::makeCalendarDate(const Date *date)
{
    return dec(date->month(), 2) + "-" + dec(date->day(), 2) + "-" + dec(date->day(), 2);
}

String DeviceDescription::FileInfo::makeDayTime(const Date *date)
{
    return dec(date->hour() > 12 ? date->hour() - 12 : date->hour(), 2) + ":" + dec(date->minutes(), 2) + (date->hour() > 12 ? "PM" : "AM");
}

double DeviceDescription::FileInfo::makeTime(const String &calendarDate, const String &dayTime)
{
    int month  = 1;
    int day    = 1;
    int year   = 1970;
    int hour   = 0;
    int minute = 0;
    {
        auto parts = calendarDate->split('-');
        if (parts->count() == 3) {
            month = parts->at(0)->toNumber<int, 10>();
            day   = parts->at(1)->toNumber<int, 10>();
            year  = parts->at(2)->toNumber<int, 10>();
        }
    }
    {
        String s = dayTime;
        bool am = s->match(Match::Tail, "AM");
        bool pm = s->match(Match::Tail, "PM");
        if (am | pm) s = s->copy(0, s->count() - 2);
        auto parts = s->split(':');
        if (parts->count() == 2) {
            hour   = parts->at(0)->toNumber<int, 10>();
            minute = parts->at(0)->toNumber<int, 10>();
        }
        if (pm) hour += 12;
    }
    return Date::compose(year, month, day, hour, minute)->time();
}

void DeviceDescription::FileInfo::establish(const String &key, const String &value)
{
    if (key->match("FileName")) fileName_ = value;
    else if (key->match("FileVersion")) fileVersion_ = value->toNumber<uint8_t>();
    else if (key->match("FileRevision")) fileRevision_ = value->toNumber<uint8_t>();
    else if (key->match("EDSVersion")) fileFormatVersion_ = value;
    else if (key->match("Description")) description_ = value;
    else if (key->match("CreationTime")) creationDayTime_ = value;
    else if (key->match("CreationDate")) creationDate_ = value;
    else if (key->match("CreatedBy")) createdBy_ = value;
    else if (key->match("ModificationTime")) changedDayTime_ = value;
    else if (key->match("ModificationDate")) changedDate_ = value;
    else if (key->match("ModifiedBy")) changedBy_ = value;
}

String DeviceDescription::FileInfo::toString() const
{
    return Format()
        << "[" << sectionName() << "]" << nl
        << "FileName=" << fileName_ << nl
        << "FileVersion=" << fileVersion_ << nl
        << "FileRevision=" << fileRevision_ << nl
        << "EDSVersion=" << fileFormatVersion_ << nl
        << "Description=" << description_ << nl
        << "CreationTime=" << creationDayTime_ << nl
        << "CreationDate=" << creationDate_ << nl
        << "CreatedBy=" << createdBy_ << nl
        << "ModificationTime=" << changedDayTime_ << nl
        << "ModificationDate=" << changedDate_ << nl
        << "ModifiedBy=" << changedBy_ << nl;
}

bool DeviceDescription::FileInfo::equals(const FileInfo *other) const
{
    return
        fileName_ == other->fileName_ &&
        fileVersion_ == other->fileVersion_ &&
        fileRevision_ == other->fileRevision_ &&
        fileFormatVersion_ == other->fileFormatVersion_ &&
        description_ == other->description_ &&
        creationDayTime_ == other->creationDayTime_ &&
        createdBy_ == other->createdBy_ &&
        changedDayTime_ == other->changedDayTime_ &&
        changedBy_ == other->changedBy_;
}

DeviceDescription::DeviceInfo::DeviceInfo():
    Section{"DeviceInfo"},
    supportedBaudRates_{Set<uint32_t>::create()}
{}

bool DeviceDescription::DeviceInfo::supportsBaudRate(uint32_t baudRate) const { return supportedBaudRates_->contains(baudRate); }
int DeviceDescription::DeviceInfo::supportedBaudRateCount() const { return supportedBaudRates_->count(); }
uint32_t DeviceDescription::DeviceInfo::supportedBaudRateAt(int i) const { return supportedBaudRates_->at(i); }

void DeviceDescription::DeviceInfo::insertSupportedBaudRate(uint32_t baudRate)
{
    supportedBaudRates_->insert(baudRate);
}

void DeviceDescription::DeviceInfo::removeSupportedBaudRate(uint32_t baudRate)
{
    supportedBaudRates_->remove(baudRate);
}

void DeviceDescription::DeviceInfo::establish(const String &key, const String &value)
{
    if (key->match("VendorName")) vendorName_ = value;
    else if (key->match("VendorNumber")) vendorNumber_ = value->toNumber<uint32_t>();
    else if (key->match("ProductName")) productName_ = value;
    else if (key->match("ProductNumber")) productNumber_ = value->toNumber<uint32_t>();
    else if (key->match("RevisionNumber")) revisionNumber_ = value->toNumber<uint32_t>();
    else if (key->match("OrderCode")) orderCode_ = value;
    else if (key->match(Match::Head, "BaudRate")) {
        int i = key->find('_');
        if (i < key->count()) {
            bool ok = false;
            uint32_t baudRate = key->select(i + 1, key->count())->toNumber<uint32_t>(&ok) * 1000U;
            if (ok && value->toNumber<int>() == 1) supportedBaudRates_->insert(baudRate);
        }
    }
    else if (key->match("SimpleBootUpMaster")) simpleBootUpMaster_ = value->toNumber<int>();
    else if (key->match("SimpleBootUpSlave")) simpleBootUpSlave_ = value->toNumber<int>();
    else if (key->match("Granularity")) granularity_ = value->toNumber<uint8_t>();
    else if (key->match("DynamicChannelsSupported")) dynamicChannels_ = value->toNumber<int>();
    else if (key->match("GroupMessaging")) groupMessaging_ = value->toNumber<int>();
    else if (key->match("NrOfRXPDO")) slotCount_ = value->toNumber<uint16_t>();
    else if (key->match("NrOfTXPDO")) signalCount_ = value->toNumber<uint16_t>();
    else if (key->match("LSS_Supported")) layerSpecificationServices_ = value->toNumber<int>();
}

String DeviceDescription::DeviceInfo::toString() const
{
    Format f;

    f << "[" << sectionName() << "]" << nl
      << "VendorName=" << vendorName_ << nl
      << "VendorNumber=0x" << hex(vendorNumber_) << nl
      << "ProductName=" << productName_ << nl
      << "ProductNumber=" << productNumber_ << nl
      << "RevisionNumber=" << revisionNumber_ << nl
      << "OrderCode=" << orderCode_ << nl;

    const int standardBaudRatesCount = 8;
    const uint32_t standardBaudRates[standardBaudRatesCount] = { 10000, 20000, 50000, 125000, 250000, 500000, 800000, 1000000 };
    for (int i = 0; i < standardBaudRatesCount; ++i) {
        uint32_t baudRate = standardBaudRates[i];
        f << "BaudRate_" << baudRate/1000 << "=" << (supportedBaudRates_->contains(baudRate) ? "1" : "0") << nl;
    }

    f << "SimpleBootUpMaster=" << int(simpleBootUpMaster_) << nl
      << "SimpleBootUpSlave=" << int(simpleBootUpSlave_) << nl
      << "Granularity=" << granularity_ << nl
      << "DynamicChannelsSupported=" << int(dynamicChannels_) << nl
      << "GroupMessaging=" << int(groupMessaging_) << nl
      << "NrOfRXPDO=" << slotCount_ << nl
      << "NrOfTXPDO=" << signalCount_ << nl
      << "LSS_Supported=" << int(layerSpecificationServices_) << nl;

    return f;
}

bool DeviceDescription::DeviceInfo::equals(const DeviceInfo *other) const
{
    return
        vendorName_ == other->vendorName_ &&
        vendorNumber_ == other->vendorNumber_ &&
        productName_ == other->productName_ &&
        productNumber_ == other->productNumber_ &&
        revisionNumber_ == other->revisionNumber_ &&
        orderCode_ == other->orderCode_ &&
        *supportedBaudRates_ == *other->supportedBaudRates_ &&
        simpleBootUpMaster_ == other->simpleBootUpMaster_ &&
        simpleBootUpSlave_ == other->simpleBootUpSlave_ &&
        granularity_ == other->granularity_ &&
        dynamicChannels_ == other->dynamicChannels_ &&
        groupMessaging_ == other->groupMessaging_ &&
        slotCount_ == other->slotCount_ &&
        signalCount_ == other->signalCount_ &&
        layerSpecificationServices_ == other->layerSpecificationServices_;
}

DeviceDescription::DummyUsage::DummyUsage():
    Section{"DummyUsage"},
    dummies_{Set<uint16_t>::create()}
{}

bool DeviceDescription::DummyUsage::dummyEnabled(uint16_t index) const
{
    return dummies_->contains(index);
}

int DeviceDescription::DummyUsage::dummyCount() const
{
    return dummies_->count();
}

uint16_t DeviceDescription::DummyUsage::dummyAt(int i) const
{
    return dummies_->at(i);
}

void DeviceDescription::DummyUsage::enableDummy(uint16_t index) const
{
    dummies_->insert(index);
}

void DeviceDescription::DummyUsage::disableDummy(uint16_t index) const
{
    dummies_->remove(index);
}

void DeviceDescription::DummyUsage::establish(const String &key, const String &value)
{
    if (key->match(Match::Head, "Dummy")) {
        String s = key->copy(5, key->count());
        uint16_t index = s->toNumber<uint16_t, 16>();
        bool enabled = value->toNumber<int>();
        if (enabled) dummies_->insert(index);
    }
}

String DeviceDescription::DummyUsage::toString() const
{
    Format f;
    f << "[" << sectionName() << "]" << nl;
    uint16_t typeIndex = 1;
    for (int i = 0; i < dummies_->count(); ++i) {
        uint16_t nextDummy = dummies_->at(i);
        for (; typeIndex < nextDummy; ++typeIndex)
            f << "Dummy" << hex(typeIndex, 4) << "=0" << nl;
        f << "Dummy" << hex(nextDummy, 4) << "=1" << nl;
        ++typeIndex;
    }
    return f;
}

bool DeviceDescription::DummyUsage::equals(const DummyUsage *other) const
{
    return *dummies_ == *other->dummies_;
}

DeviceDescription::Comments::Comments():
    Section{"Comments"},
    lines_{StringList::create()}
{}

void DeviceDescription::Comments::establish(const String &key, const String &value)
{
    if (key->match("Lines"));
    else if (key->match(Match::Head, "Line")) lines_->append(value);
}

String DeviceDescription::Comments::text() const
{
    return lines_->join("\n");
}

void DeviceDescription::Comments::setText(const String &text)
{
    lines_ = StringList::create();
    for (const String &line: LineSource::open(text))
        lines_->append(line);
}

String DeviceDescription::Comments::toString() const
{
    Format f;
    f << "[" << sectionName() << "]" << nl;
    f << "Lines=" << lines_->count() << nl;
    for (int i = 0; i < lines_->count(); ++i)
        f << "Line" << i << "=" << lines_->at(i) << nl;
    return f;
}

bool DeviceDescription::Comments::equals(const Comments *other) const
{
    return *lines_ == *other->lines_;
}

DeviceDescription::DictionarySection::DictionarySection(const String &name):
    Section{name},
    entries_{Entries::create()}
{}

int DeviceDescription::DictionarySection::entryCount() const
{
    return entries_->count();
}

const DeviceDescription::EntryInfo *DeviceDescription::DictionarySection::entryAt(int i) const
{
    return entries_->valueAt(i);
}

bool DeviceDescription::DictionarySection::insertEntry(EntryInfo *entry)
{
    return entries_->insert(entry->index(), entry);
}

void DeviceDescription::DictionarySection::removeEntry(EntryInfo *entry)
{
    entries_->remove(entry->index());
}

void DeviceDescription::DictionarySection::enterSection(const String &sectionName)
{
    if (sectionName->match(Match::Find, "sub")) {
        if (currentEntry_) currentEntry_->enterSection(sectionName);
    }
    else {
        bool ok = false;
        uint16_t index = sectionName->toNumber<uint16_t, 16>(&ok);
        if (ok) {
            auto newEntry = EntryInfo::create(index);;
            entries_->establish(index, newEntry);
            currentEntry_ = newEntry;
        }
    }
}

void DeviceDescription::DictionarySection::establish(const String &key, const String &value)
{
    if (currentEntry_) currentEntry_->establish(key, value);
}

String DeviceDescription::DictionarySection::toString() const
{
    Format f;
    f << "[" << sectionName() << "]" << nl
      << "SupportedObjects=" << entries_->count() << nl;
    for (int i = 0; i < entries_->count(); ++i)
        f << (i + 1) << "=0x" << hex(entries_->keyAt(i), 4) << nl;
    for (int i = 0; i < entries_->count(); ++i)
        f << nl << entries_->valueAt(i)->toString();
    return f;
}

bool DeviceDescription::DictionarySection::equals(const DictionarySection *other) const
{
    if (entries_->count() != other->entries_->count()) return false;
    for (int i = 0; i < entries_->count(); ++i) {
        if (!entries_->valueAt(i)->equals(other->entries_->valueAt(i)))
            return false;
    }
    return true;
}

Ref<DeviceDescription::EntryInfo> DeviceDescription::EntryInfo::create(uint16_t index, uint8_t subIndex)
{
    return new EntryInfo{index, subIndex};
}

DeviceDescription::EntryInfo::EntryInfo(uint16_t index, uint8_t subIndex):
    index_{index},
    subIndex_{subIndex},
    subEntries_{SubEntries::create()}
{}

AccessType DeviceDescription::EntryInfo::accessType() const
{
    if (accessType_->match("ro")) return AccessType::ReadOnly;
    else if (accessType_->match("wo")) return AccessType::WriteOnly;
    else if (accessType_->match("rw")) return AccessType::ReadWrite;
    else if (accessType_->match("rwr")) return AccessType::ProcessInput;
    else if (accessType_->match("rww")) return AccessType::ProcessOutput;
    return AccessType::Undefined;
}

void DeviceDescription::EntryInfo::setAccessType(AccessType newValue)
{
    switch (newValue) {
        case AccessType::ReadOnly     : accessType_ = "ro"; break;
        case AccessType::WriteOnly    : accessType_ = "wo"; break;
        case AccessType::ReadWrite    : accessType_ = "rw"; break;
        case AccessType::ProcessInput : accessType_ = "rwr"; break;
        case AccessType::ProcessOutput: accessType_ = "rww"; break;
        default                       : accessType_ =""; break;
    };
}

bool DeviceDescription::EntryInfo::insertSubEntry(EntryInfo *entry)
{
    CC_ASSERT(entry->index() == index());
    if (entry->index() != index()) return false;
    return subEntries_->insert(entry->subIndex(), entry);
}

void DeviceDescription::EntryInfo::removeSubEntry(EntryInfo *entry)
{
    CC_ASSERT(entry->index() == index());
    if (entry->index() == index())
        subEntries_->remove(entry->subIndex());
}

void DeviceDescription::EntryInfo::enterSection(const String &sectionName)
{
    auto parts = sectionName->toLower()->split("sub");
    if (parts->count() == 2) {
        bool ok = false;
        uint16_t index = parts->at(0)->toNumber<uint16_t, 16>(&ok);
        if (ok) {
            uint8_t subIndex = parts->at(1)->toNumber<uint8_t, 16>(&ok);
            if (ok) {
                auto newSubEntry = EntryInfo::create(index, subIndex);
                subEntries_->establish(subIndex, newSubEntry);
                currentSubEntry_ = newSubEntry;
            }
        }
    }
}

void DeviceDescription::EntryInfo::establish(const String &key, const String &value)
{
    if (currentSubEntry_) {
        currentSubEntry_->establish(key, value);
    }
    else {
        if (key->match("SubNumber")) /*ignored*/;
        else if (key->match("ParameterName")) parameterName_ = value;
        else if (key->match("ObjectType")) objectCode_ = static_cast<ObjectCode>(value->toNumber<uint8_t>());
        else if (key->match("DataType")) dataType_ = static_cast<DataType>(value->toNumber<uint16_t>());
        else if (key->match("LowLimit")) lowLimit_ = value;
        else if (key->match("HighLimit")) highLimit_ = value;
        else if (key->match("AccessType")) accessType_ = value;
        else if (key->match("DefaultValue")) defaultValue_ = value;
        else if (key->match("PDOMapping")) mappable_ = value->toNumber<int>();
        else if (key->match("ObjFlags")) objectFlags_ = static_cast<ObjectFlags>(value->toNumber<uint32_t>());
        else if (key->match("ParameterValue")) parameterValue_ = value;
        else if (key->match("UploadFile")) uploadPath_ = value;
        else if (key->match("DownloadFile")) downloadPath_ = value;
        else if (key->match("Denotation")) denotation_ = value;
    }
}

String DeviceDescription::EntryInfo::toString() const
{
    Format f;
    f << "[" << hex(index_, 4) << "]" << nl
      << "ParameterName=" << parameterName_ << nl
      << "ObjectType=0x" << hex(static_cast<uint8_t>(objectCode_)) << nl;

    if (dataType_ != DataType::Undefined) f << "DataType=0x" << hex(static_cast<uint16_t>(dataType_), 4) << nl;
    if (accessType_ != "") f << "AccessType=" << accessType_ << nl;
    if (defaultValue_ != "") f << "DefaultValue=" << defaultValue_ << nl;
    if (lowLimit_ != "") f << "LowLimit=" << lowLimit_ << nl;
    if (highLimit_ != "") f << "HighLimit=" << highLimit_ << nl;
    if (dataType_ != DataType::Undefined) f << "PDOMapping=" << (mappable_ ? "1" : "0") << nl;
    if (objectFlags_ != ObjectFlags::None) f << "ObjFlags=" << static_cast<uint32_t>(objectFlags_) << nl;

    if (parameterValue_ != "") f << "ParameterValue=" << parameterValue_ << nl;
    if (uploadPath_ != "") f << "UploadFile=" << uploadPath_ << nl;
    if (downloadPath_ != "") f << "DownloadFile=" << downloadPath_ << nl;
    if (denotation_ != "") f << "Denotation=" << denotation_ << nl;

    if (subEntries_->count() > 0) {
        f << "SubNumber=" << subEntries_->count() << nl;

        for (int i = 0; i < subEntries_->count(); ++i) {
            const EntryInfo *sub = subEntries_->valueAt(i);
            f << nl
              << "[" << hex(sub->index_, 4) << "sub" << hex(sub->subIndex_) << "]" << nl
              << "ParameterName=" << sub->parameterName_ << nl
              << "ObjectType=0x" << hex(static_cast<uint8_t>(sub->objectCode_)) << nl
              << "DataType=0x" << hex(static_cast<uint16_t>(sub->dataType_), 4) << nl
              << "AccessType=" << sub->accessType_ << nl;
            if (sub->defaultValue_ != "") f << "DefaultValue=" << sub->defaultValue_ << nl;
            if (sub->lowLimit_ != "") f << "LowLimit=" << sub->lowLimit_ << nl;
            if (sub->highLimit_ != "") f << "HighLimit=" << sub->highLimit_ << nl;
            f << "PDOMapping=" << (sub->mappable_ ? "1" : "0") << nl;
            if (sub->objectFlags_ != ObjectFlags::None) f << "ObjFlags=" << static_cast<uint32_t>(sub->objectFlags_) << nl;

            if (parameterValue_ != "") f << "ParameterValue=" << parameterValue_ << nl;
            if (uploadPath_ != "") f << "UploadFile=" << uploadPath_ << nl;
            if (downloadPath_ != "") f << "DownloadFile=" << downloadPath_ << nl;
            if (denotation_ != "") f << "Denotation=" << denotation_ << nl;
        }
    }

    return f;
}

bool DeviceDescription::EntryInfo::equals(const EntryInfo *other) const
{
    auto stringValueEquals = [](const String &a, const String &b) -> bool {
        if (a == b) return true;
        bool okA = false, okB = false;
        if (a->contains('.') || b->contains('.')) {
            double valueA = a->toNumber<double>(&okA);
            double valueB = b->toNumber<double>(&okB);
            return okA == okB && valueA == valueB;
        }
        else if (a->contains('-') || b->contains('-')){
            int64_t valueA = a->toNumber<int64_t>(&okA);
            int64_t valueB = b->toNumber<int64_t>(&okB);
            return okA == okB && valueA == valueB;
        }
        uint64_t valueA = a->toNumber<uint64_t>(&okA);
        uint64_t valueB = b->toNumber<uint64_t>(&okB);
        return okA == okB && valueA == valueB;
    };

    return
        parameterName_ == other->parameterName_ &&
        objectCode_ == other->objectCode_ &&
        dataType_ == other->dataType_ &&
        stringValueEquals(lowLimit_, other->lowLimit_) &&
        stringValueEquals(highLimit_, other->highLimit_) &&
        accessType_ == other->accessType_ &&
        stringValueEquals(defaultValue_, other->defaultValue_) &&
        mappable_ == other->mappable_ &&
        objectFlags_ == other->objectFlags_ &&
        [=](){
            if (subEntries_->count() != other->subEntries_->count()) return false;
            for (int i = 0; i < subEntries_->count(); ++i) {
                if (!subEntries_->valueAt(i)->equals(other->subEntries_->valueAt(i)))
                    return false;
            }
            return true;
        }();
}

DeviceDescription::MandatoryObjects::MandatoryObjects():
    DictionarySection{"MandatoryObjects"}
{}

DeviceDescription::OptionalObjects::OptionalObjects():
    DictionarySection{"OptionalObjects"}
{}

DeviceDescription::ManufacturerObjects::ManufacturerObjects():
    DictionarySection{"ManufacturerObjects"}
{}

}} // namespace cc::can
