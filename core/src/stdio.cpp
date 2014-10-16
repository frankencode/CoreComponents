/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ThreadLocalSingleton"
#include "stdio"

namespace flux
{

class StdIo: public Object, public ThreadLocalSingleton<StdIo>
{
public:
	inline File *in() {
		if (!in_) in_ = File::open(File::StandardInput, File::ReadOnly);
		return in_;
	}

	inline File *out() {
		if (!out_) out_ = File::open(File::StandardOutput, File::WriteOnly);
		return out_;
	}

	inline File *err() {
		if (!err_) err_ = File::open(File::StandardError, File::WriteOnly);
		return err_;
	}

	inline LineSource *lineInput() {
		if (!lineInput_) lineInput_ = LineSource::open(in());
		return lineInput_;
	}

	Ref<File> in_;
	Ref<File> out_;
	Ref<File> err_;
	Ref<LineSource> lineInput_;
};

File *in() { return StdIo::instance()->in(); }
File *out() { return StdIo::instance()->out(); }
File *err() { return StdIo::instance()->err(); }

LineSource *lineInput() { return StdIo::instance()->lineInput(); }

} // namespace flux
