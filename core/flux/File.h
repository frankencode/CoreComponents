/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_FILE_H
#define FLUX_FILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "String.h"
#include "SystemStream.h"
#include "FileStatus.h"

namespace flux
{

class File: public SystemStream
{
public:
	enum Type {
		Regular      = S_IFREG,
		Directory    = S_IFDIR,
		CharDevice   = S_IFCHR,
		BlockDevice  = S_IFBLK,
		Fifo         = S_IFIFO,
		Link         = S_IFLNK,
		Socket       = S_IFSOCK,
	};

	enum Flags {
		SetUserId  = S_ISUID,
		SetGroupId = S_ISGID,
		StickyBit  = S_ISVTX
	};

	enum OpenFlags {
		ReadOnly  = O_RDONLY,
		WriteOnly = O_WRONLY,
		ReadWrite = O_RDWR,
		Append    = O_APPEND
	};

	enum AccessFlags {
		Readable    = R_OK,
		Writeable   = W_OK,
		Executable  = X_OK,
		Exists      = F_OK
	};

	enum ModeFlags {
		UserRead   = 0400,
		UserWrite  = 0200,
		UserExec   = 0100,
		GroupRead  = 0040,
		GroupWrite = 0020,
		GroupExec  = 0010,
		OtherRead  = 0004,
		OtherWrite = 0002,
		OtherExec  = 0001
	};

	enum SeekMethod {
		SeekBegin   = SEEK_SET,
		SeekCurrent = SEEK_CUR,
		SeekEnd     = SEEK_END
	};

	enum StandardStreams {
		StandardInput  = 0,
		StandardOutput = 1,
		StandardError  = 2
	};

	static Ref<File> open(String path, int openFlags = ReadOnly);
	static Ref<File> tryOpen(String path, int openFlags = ReadOnly);
	static Ref<File> open(int fd, int openFlags);
	static Ref<File> temp(int openFlags = ReadWrite);

	String path() const;
	String name() const;
	Ref<FileStatus> status() const;

	int openFlags() const;

	void truncate(off_t length);
	void unlinkOnExit();
	void unlinkOnThreadExit();
	void unlinkWhenDone();

	off_t seek(off_t distance, int method = SeekBegin);
	bool seekable() const;

	String map() const;

	void sync();
	void dataSync();

	static bool access(String path, int flags);
	static bool exists(String path);
	static bool create(String path, int mode = 0644);
	static bool chown(String path, uid_t ownerId, gid_t groupId);
	static bool rename(String path, String newPath);
	static bool link(String path, String newPath);
	static bool unlink(String path);
	static bool symlink(String path, String newPath);
	static String readlink(String path);
	static String resolve(String path);

	static String createUnique(String path, int mode = 0644, char placeHolder = 'X');
	static bool establish(String path, int fileMode = 0644, int dirMode = 0755);
	static String lookup(String fileName, StringList *dirs = 0, int accessFlags = Executable);

	static Ref<FileStatus> status(String path);
	static Ref<FileStatus> unresolvedStatus(String path);

	static String load(String path);
	static void save(String path, String text);

private:
	static int translateOpenFlags(int openFlags);

	File(String path, int openFlags, int fd);
	~File();

	String path_;
	int openFlags_;
	bool unlinkWhenDone_;
};

} // namespace flux

#endif // FLUX_FILE_H
