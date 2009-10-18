/*
 * File.hpp -- file I/O, status and permissions
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FILE_HPP
#define PONA_FILE_HPP

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "atoms"
#include "String.hpp"
#include "SystemStream.hpp"
#include "FileStatus.hpp"

namespace pona
{

class PONA_API File: public SystemStream
{
public:
	enum Type {
		Regular      = S_IFREG,
		Directory    = S_IFDIR,
		CharDevice   = S_IFCHR,
		BlockDevice  = S_IFBLK,
		Fifo         = S_IFIFO,
		SymbolicLink = S_IFLNK,
		Socket       = S_IFSOCK
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
	
	File(String path);
	File(int fd);
	
	String path() const;
	String name() const;
	int openFlags() const;
	
	bool access(int flags) const;
	bool exists() const;
	void create(int mask = 0644);
	void unlink();
	
	void createUnique(int mask = 0644, Char placeHolder = 'X');
	void truncate(off_t length);
	void unlinkOnExit();
	void unlinkOnThreadExit();
	
	void open(int flags = Read|Write);
	
	off_t seek(off_t distance, int method = SeekBegin);
	void seekSet(off_t distance);
	void seekMove(off_t distance);
	off_t seekTell();
	off_t size();
	
	Ref<FileStatus, Owner> status() const;
	
	void sync();
	void dataSync();
	
private:
	String path_;
	int openFlags_;
};

} // namespace pona

#endif // PONA_FILE_HPP
