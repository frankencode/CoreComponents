/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Config.h>
#include <flux/Date.h>
#include <flux/File.h>
#include <flux/Dir.h>
#include <flux/Process.h>
#include <flux/User.h>
#include <flux/Group.h>
#include <flux/Archive.h>

using namespace flux;

int main(int argc, char **argv)
{
	try {
		Ref<YasonObject> prototype = YasonObject::create();
		prototype->insert("list", false);
		prototype->insert("status", false);
		prototype->insert("explode", false);
		prototype->insert("bundle", false);
		Ref<Config> options = Config::read(argc, argv, prototype);
		if (options->value("list")) {
			for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);
				Ref<Archive> archive = Archive::open(path);
				for (Ref<ArchiveEntry> entry; archive->read(&entry);) {
					fout() << entry->path() << nl;
				}
			}
		}
		else if (options->value("status")) {
			for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);
				Ref<Archive> archive = Archive::open(path);
				for (Ref<ArchiveEntry> entry; archive->read(&entry);) {
					Format status = fout();
					status << oct(entry->mode()) << " ";
					if (entry->userName() != "") status << entry->userName() << " ";
					else status << entry->userId() << " ";
					if (entry->groupName() != "") status << entry->groupName() << " ";
					else status << entry->groupId() << " ";
					status << entry->size() << " ";
					status << Date::create(entry->lastModified()) << " ";
					status << entry->path() << nl;
				}
			}
		}
		else if (options->value("explode")) {
			for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);
				Ref<Archive> archive = Archive::open(path);
				for (Ref<ArchiveEntry> entry; archive->read(&entry);) {
					fout() << entry->path() << nl;
					if (entry->type() == ArchiveEntry::Directory) {
						if (!Dir::create(entry->path(), entry->mode()))
							FLUX_SYSTEM_EXCEPTION;
					}
					else if (entry->type() == ArchiveEntry::Link) {
						if (!File::link(entry->linkPath(), entry->path()))
							FLUX_SYSTEM_EXCEPTION;
					}
					else if (entry->type() == ArchiveEntry::Symlink) {
						if (!File::symlink(entry->linkPath(), entry->path()))
							FLUX_SYSTEM_EXCEPTION;
					}
					else if (entry->type() == ArchiveEntry::Regular) {
						if (!File::create(entry->path(), entry->mode()))
							FLUX_SYSTEM_EXCEPTION;
						if (Process::isSuperUser()) {
							uid_t userId = entry->userId();
							gid_t groupId = entry->groupId();
							if (entry->userName() != "")
								userId = User::lookup(entry->userName())->id();
							if (entry->groupName() != "")
								groupId = Group::lookup(entry->groupName())->id();
							if (userId != 0 || groupId != 0) {
								if (!File::chown(entry->path(), userId, groupId))
									FLUX_SYSTEM_EXCEPTION;
							}
						}
						Ref<File> file = File::open(entry->path(), File::WriteOnly);
						file->write(ByteArray::wrap(archive->data()->data() + entry->offset(), entry->size()));
					}
				}
			}
		}
		else if (options->value("bundle")) {
			/*for (int i = 0; i < options->arguments()->size(); ++i) {
				String path = options->arguments()->at(i);

			}*/
		}
	}
	catch (HelpException &) {
		fout(
			"Usage: %% [OPTION]... [FILE|DIR]...\n"
			"Pack, unpack or list the contents of archive files.\n"
			"\n"
			"Options:\n"
			"  -list     list contents of given archive files\n"
			"  -status   show archived file status for each archive member\n"
			"  -explode  unpack archive files\n"
			"  -bundle   pack each directory into an archive file\n"
		) << String(argv[0])->fileName();
		return 1;
	}
	catch (UserException &ex) {
		fout() << ex.message() << nl;
		return 1;
	}

	return 0;
}
