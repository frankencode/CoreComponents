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

namespace ftl
{

class FileStatus;

class File: public SystemStream
{
public:
	enum Type {
		Regular      = S_IFREG,
		Directory    = S_IFDIR,
		CharDevice   = S_IFCHR,
		BlockDevice  = S_IFBLK,
		Fifo         = S_IFIFO,
		SymbolicLink = S_IFLNK,
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

	enum CreateFlags {
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

	inline static Ref<File, Owner> newInstance(String path, int openFlags = 0) { return new File(path, openFlags); }
	inline static Ref<File, Owner> newInstance(int fd, int openFlags = Read|Write){ return new File(fd, openFlags); }
	~File();

	String path() const;
	String name() const;
	int openFlags() const;

	bool access(int flags) const;
	bool exists() const;
	void create(int mode = 0644);
	void link(const char *newPath);
	void unlink();

	void createUnique(int mode = 0644, char placeHolder = 'X');
	void truncate(off_t length);
	void unlinkOnExit();
	void unlinkOnThreadExit();
	void unlinkWhenDone();

	void open(int flags = Read);

	static Ref<File, Owner> temp();
	void establish(int fileMode = 0644, int dirMode = 0755);
	String load();
	void save(String text);

	off_t seek(off_t distance, int method = SeekBegin);
	void seekSet(off_t distance);
	void seekMove(off_t distance);
	off_t seekTell();
	off_t size();

	void sync();
	void dataSync();
	static void syncAll();

	static String lookup(String fileName, Ref<StringList> dirs = 0, int accessFlags = Execute);

	Ref<FileStatus> status() const;

protected:
	File(String path, int openFlags = 0);
	File(int fd, int openFlags = Read|Write);

	String path_;
	int openFlags_;
	bool unlinkWhenDone_;
	mutable Ref<FileStatus, Owner> status_;
};

} // namespace ftl

#endif // FTL_FILE_HPP
