/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTAR_TARWRITER_H
#define FLUXTAR_TARWRITER_H

#include <flux/FileStatus.h>
#include <flux/Map.h>
#include "ArchiveWriter.h"

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
