/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/Process.h>
#include <fkit/FileStatus.h>
#include <fkit/File.h>
#include <fkit/Dir.h>
#include "BuildPlan.h"
#include "Shell.h"

namespace fmake
{

Shell::Shell(BuildPlan *plan)
	: plan_(plan)
{}

String Shell::beautify(String command)
{
	if (plan()->options() & BuildPlan::Bootstrap) {
		return command
			->replace(plan()->sourcePrefix(), String("$SOURCE"))
			->replace(Process::cwd(), String("$PWD"));
	}
	return command;
}

bool Shell::run(String command)
{
	ferr() << beautify(command) << nl;
	if (plan()->options() & BuildPlan::Simulate) return true;
	return Process::start(command)->wait() == 0;
}

Ref<FileStatus> Shell::fileStatus(String path)
{
	if (plan()->options() & BuildPlan::Blindfold) return FileStatus::read();
	return FileStatus::read(path);
}

bool Shell::mkdir(String path)
{
	if (!fileStatus(path)->exists())
		ferr("mkdir -p %%\n") << path;
	if (plan()->options() & BuildPlan::Simulate) return true;
	return Dir::establish(path);
}

bool Shell::rmdir(String path)
{
	if (fileStatus(path)->exists())
		ferr("rmdir %%\n") << path;
	if (plan()->options() & BuildPlan::Simulate) return true;
	return Dir::unlink(path);
}

bool Shell::symlink(String path, String newPath)
{
	ferr("ln -sf %% %%\n") << path << newPath;
	if (plan()->options() & BuildPlan::Simulate) return true;
	File::unlink(newPath);
	return File::symlink(path, newPath);
}

bool Shell::install(String sourcePath, String destPath)
{
	String destDirPath = destPath->reducePath();
	bool destDirMissing = destDirPath != "" && !fileStatus(destDirPath)->exists();
	if (destDirMissing) ferr("install -d %%\n") << destDirPath;

	ferr("install %% %%\n") << sourcePath << destPath;

	if (plan()->options() & BuildPlan::Simulate) return true;

	try {
		if (destDirMissing) Dir::establish(destDirPath) || FKIT_SYSTEM_EXCEPTION;
		Ref<File> source = File::open(sourcePath);
		Ref<FileStatus> sourceStatus = FileStatus::read(sourcePath);
		if (File::exists(destPath)) File::unlink(destPath);
		File::create(destPath, sourceStatus->mode()) || FKIT_SYSTEM_EXCEPTION;
		Ref<File> sink = File::open(destPath, File::WriteOnly);
		sink->truncate(0);
		sink->write(source->map());
	}
	catch (SystemException &ex) {
		ferr("%%\n") << ex.what();
		return false;
	}

	return true;
}

bool Shell::unlink(String path)
{
	if (File::unresolvedStatus(path)->exists()) {
		if (plan()->options() & BuildPlan::Simulate) {
			ferr("rm -f %%\n") << path;
			return true;
		}
		ferr("rm %%\n") << path;
		try {
			File::unlink(path) || FKIT_SYSTEM_EXCEPTION;
		}
		catch (SystemException &ex) {
			ferr("%%\n") << ex.message();
			return false;
		}
	}
	return true;
}

} // namespace fmake
