/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_TARWRITER_H
#define FLUXTAR_TARWRITER_H

#include <flux/FileStatus>
#include <flux/Map>
#include <flux/tar/ArchiveWriter>

namespace flux {
namespace tar {

class TarWriter: public ArchiveWriter
{
public:
	static Ref<TarWriter> open(Stream *sink);

	void writeFile(String path);

private:
	TarWriter(Stream *sink);
	~TarWriter();
	void writeFile(String path, FileStatus *status);
	void writePadding(off_t unpaddedSize);

	Ref<Stream> sink_;

	class FileId {
	public:
		FileId()
		{}

		FileId(FileStatus *status)
			: deviceId_(status->deviceId()),
			  inodeNumber_(status->inodeNumber())
		{}

		inline bool operator<(const FileId &b) const {
			if (deviceId_ < b.deviceId_) return true;
			if (b.deviceId_ < deviceId_) return false;
			return inodeNumber_ < b.inodeNumber_;
		}

	private:
		dev_t deviceId_;
		ino_t inodeNumber_;
	};

	typedef Map<FileId, String> HardLinks;

	String zero_;
	Ref<HardLinks> hardLinks_;
	Ref<FileStatus> longPathStatus_;
	Ref<FileStatus> longLinkStatus_;
};

}} // namespace flux::tar

#endif // FLUXTAR_TARWRITER_H
