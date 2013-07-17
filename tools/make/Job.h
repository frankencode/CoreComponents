/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_JOB_H
#define FMAKE_JOB_H

#include <fkit/String.h>
#include <fkit/Channel.h>

namespace fmake
{

using namespace fkit;

class JobServer;

class Job: public Object
{
public:
	inline static Ref<Job> create(String command) {
		return new Job(command);
	}

	inline String command() const { return command_; }

	inline int status() const { return status_; }
	inline String outputText() const { return outputText_; }

private:
	friend class JobServer;

	Job(String command)
		: command_(command),
		  status_(-1)
	{}

	String command_;

	int status_;
	String outputText_;
};

typedef Channel< Ref<Job> > JobChannel;

} // namespace fmake

#endif // FMAKE_JOB_H
