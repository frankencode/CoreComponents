/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Date>
#include <flux/File>
#include <flux/Dir>
#include <flux/Process>
#include <flux/User>
#include <flux/Group>
#include <flux/exceptions>
#include "unpack.h"

namespace fluxtar
{

void list(ArchiveReader *archive)
{
	for (Ref<ArchiveEntry> entry; archive->readHeader(&entry); archive->skipData(entry))
		fout() << entry->path() << nl;
}

void status(ArchiveReader *archive)
{
	for (Ref<ArchiveEntry> entry; archive->readHeader(&entry); archive->skipData(entry)) {
		Format status = fout();
		status << oct(entry->mode()) << "\t";
		if (entry->userName() != "") status << entry->userName() << "\t";
		else status << entry->userId() << "\t";
		if (entry->groupName() != "") status << entry->groupName() << "\t";
		else status << entry->groupId() << "\t";
		status << entry->size() << "\t";
		status << Date::create(entry->lastModified()) << "\t";
		status << entry->path() << nl;
	}
}

void unpack(ArchiveReader *archive, bool verbose)
{
	for (Ref<ArchiveEntry> entry; archive->readHeader(&entry);) {
		if (verbose) fout() << entry->path() << nl;
		if (entry->type() == ArchiveEntry::Directory) {
			if (entry->path() == "." || entry->path() == "./") ;
			else Dir::establish(entry->path(), entry->mode());
		}
		else if (entry->type() == ArchiveEntry::Link) {
			File::link(entry->linkPath(), entry->path());
				FLUX_SYSTEM_RESOURCE_ERROR(errno, entry->linkPath());
		}
		else if (entry->type() == ArchiveEntry::Symlink) {
			File::symlink(entry->linkPath(), entry->path());
		}
		else if (entry->type() == ArchiveEntry::Regular) {
			File::create(entry->path(), entry->mode());
			if (Process::isSuperUser()) {
				uid_t userId = entry->userId();
				gid_t groupId = entry->groupId();
				if (entry->userName() != "")
					userId = User::lookup(entry->userName())->id();
				if (entry->groupName() != "")
					groupId = Group::lookup(entry->groupName())->id();
				if (userId != 0 || groupId != 0)
					File::chown(entry->path(), userId, groupId);
			}
			if (entry->size() > 0) {
				Ref<File> file = File::open(entry->path(), File::WriteOnly);
				archive->readData(entry, file);
			}
			Ref<FileStatus> status = FileStatus::read(entry->path());
			status->setTimes(status->lastAccess(), entry->lastModified());
		}
	}
}

} // namespace fluxtar
