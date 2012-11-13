/*
 * File.hpp -- file I/O
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_FILE_HPP
#define FTL_FILE_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "atoms"
#include "String.hpp"
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
		SeekBegin   = 1,
		SeekCurrent = 2,
		SeekEnd     = 3
	};

	enum StandardStreams {
		StandardInput  = 0,
		StandardOutput = 1,
		StandardError  = 2
	};

	static bool access(String path, int flags);
	static bool exists(String path);
	static bool create(String path, int mode = 0644);
	static bool link(String path, String newPath);
	static bool unlink(String path);
	static bool symlink(String path, String newPath);
	static String readlink(String path);
	static String resolve(String path);

	static String createUnique(String path, int mode = 0644, char placeHolder = 'X');
	static bool establish(String path, int fileMode = 0644, int dirMode = 0755);
	static String lookup(String fileName, Ref<StringList> dirs = 0, int accessFlags = Execute);

	static Ref<FileStatus, Owner> status(String path);
	static Ref<FileStatus, Owner> unresolvedStatus(String path);

	static String load(String path);
	static void save(String path, String text);

	inline static Ref<File, Owner> open(String path, int openFlags = Read) { return new File(path, openFlags); }
	inline static Ref<File, Owner> open(int fd, int openFlags = Read|Write) { return new File(fd, openFlags); }
	static Ref<File, Owner> temp(int openFlags = Read|Write);
	~File();

	String path() const;
	String name() const;
	Ref<FileStatus, Owner> status() const;

	int openFlags() const;

	void truncate(off_t length);
	void unlinkOnExit();
	void unlinkOnThreadExit();
	void unlinkWhenDone();

	off_t seek(off_t distance, int method = SeekBegin);
	void seekSet(off_t distance);
	void seekMove(off_t distance);
	off_t seekTell();

	void sync();
	void dataSync();

protected:
	File(String path, int openFlags = 0);
	File(int fd, int openFlags = Read|Write);

	String path_;
	int openFlags_;
	bool unlinkWhenDone_;
};

} // namespace ftl

#endif // FTL_FILE_HPP
