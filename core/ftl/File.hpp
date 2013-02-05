 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_FILE_HPP
#define FTL_FILE_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "string.hpp"
#include "SystemStream.hpp"
#include "FileStatus.hpp"

namespace ftl
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

	enum AccessFlags {
		Read    = R_OK,
		Write   = W_OK,
		Execute = X_OK,
		Exists  = F_OK
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

	static hook<File> open(string path, int openFlags = Read);
	static hook<File> tryOpen(string path, int openFlags = Read);
	static hook<File> open(int fd, int openFlags);
	static hook<File> temp(int openFlags = Read|Write);

	string path() const;
	string name() const;
	hook<FileStatus> status() const;

	int openFlags() const;

	void truncate(off_t length);
	void unlinkOnExit();
	void unlinkOnThreadExit();
	void unlinkWhenDone();

	off_t seek(off_t distance, int method = SeekBegin);
	bool seekable() const;

	string map() const;

	void sync();
	void dataSync();

	static bool access(string path, int flags);
	static bool exists(string path);
	static bool create(string path, int mode = 0644);
	static bool link(string path, string newPath);
	static bool unlink(string path);
	static bool symlink(string path, string newPath);
	static string readlink(string path);
	static string resolve(string path);

	static string createUnique(string path, int mode = 0644, char placeHolder = 'X');
	static bool establish(string path, int fileMode = 0644, int dirMode = 0755);
	static string lookup(string fileName, StringList *dirs = 0, int accessFlags = Execute);

	static hook<FileStatus> status(string path);
	static hook<FileStatus> unresolvedStatus(string path);

	static string load(string path);
	static void save(string path, string text);

private:
	static int translateOpenFlags(int openFlags);

	File(string path, int openFlags, int fd);
	~File();

	string path_;
	int openFlags_;
	bool unlinkWhenDone_;
};

} // namespace ftl

#endif // FTL_FILE_HPP
