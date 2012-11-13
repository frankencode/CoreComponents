#include <ftl/Process.hpp>
#include <ftl/File.hpp>
#include <ftl/ByteDecoder.hpp>
#include "XAuthFile.hpp"

namespace ftl
{

XAuthFile::XAuthFile(String path)
	: path_(path),
	  records_(XAuthRecords::create())
{
	if (path_ == "") path_ = Process::env("XAUTHORITY");
	if (path_ == "") return;

	Ref<File, Owner> file = File::open(path_);

	Ref<ByteDecoder, Owner> source = ByteDecoder::open(file);
	source->setEndian(BigEndian);

	while (source->hasMore()) {
		Ref<XAuthRecord, Owner> record = new XAuthRecord;

		int family = source->readUInt16();
		if (family == 0) family = AF_INET;
		else if (family == 1) family = AF_INET6;
		else if (family == 256) family = AF_LOCAL;

		record->family = family;
		record->host = source->read(source->readUInt16());
		record->display = source->read(source->readUInt16())->toInt();
		record->protocol = source->read(source->readUInt16());
		record->data = source->read(source->readUInt16());

		records_->append(record);
	}
}

} // namespace ftl
