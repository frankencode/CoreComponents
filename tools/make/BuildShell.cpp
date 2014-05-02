/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Process.h>
#include <flux/FileStatus.h>
#include <flux/File.h>
#include <flux/Dir.h>
#include "BuildPlan.h"
#include "BuildShell.h"

namespace fluxmake
{

BuildShell::BuildShell(BuildPlan *plan)
	: plan_(plan)
{}

String BuildShell::beautify(String command)
{
	if (plan()->options() & BuildPlan::Bootstrap) {
		return command
			->replace(plan()->sourcePrefix(), String("$SOURCE"))
			->replace(Process::cwd(), String("$PWD"));
	}
	return command;
}

bool BuildShell::run(String command)
{
	fout() << beautify(command) << nl;
	if (plan()->options() & BuildPlan::Simulate) return true;
	return Process::start(command)->wait() == 0;
}

Ref<FileStatus> BuildShell::fileStatus(String path)
{
	if (plan()->options() & BuildPlan::Blindfold) return FileStatus::read();
	return FileStatus::read(path);
}

void BuildShell::mkdir(String path)
{
	if (!fileStatus(path)->exists())
		fout("mkdir -p %%\n") << path;
	if (plan()->options() & BuildPlan::Simulate) return;
	Dir::establish(path);
}

void BuildShell::rmdir(String path)
{
	bool exists = fileStatus(path)->exists();
	if (exists) fout("rmdir %%\n") << path;
	if (plan()->options() & BuildPlan::Simulate) return;
	if (exists) try { Dir::unlink(path); } catch (SystemError &) { /*FIXME, directory might not be empty */ }
}

void BuildShell::symlink(String path, String newPath)
{
	fout("ln -sf %% %%\n") << path << newPath;
	if (plan()->options() & BuildPlan::Simulate) return;
	try { File::unlink(newPath); } catch (SystemError &) {}
	File::symlink(path, newPath);
}

bool BuildShell::install(String sourcePath, String destPath)
{
	String destDirPath = destPath->reducePath();
	bool destDirMissing = destDirPath != "" && !fileStatus(destDirPath)->exists();
	if (destDirMissing) fout("install -d %%\n") << destDirPath;

	fout("install %% %%\n") << sourcePath << destPath;

	if (plan()->options() & BuildPlan::Simulate) return true;

	try {
		if (destDirMissing) Dir::establish(destDirPath);
		Ref<File> source = File::open(sourcePath);
		Ref<FileStatus> sourceStatus = FileStatus::read(sourcePath);
		if (File::exists(destPath)) File::unlink(destPath);
		File::create(destPath, sourceStatus->mode());
		Ref<File> sink = File::open(destPath, File::WriteOnly);
		sink->truncate(0);
		sink->write(source->map());
	}
	catch (SystemError &ex) {
		fout("%%\n") << ex.what();
		return false;
	}

	return true;
}

bool BuildShell::unlink(String path)
{
	if (File::unresolvedStatus(path)->exists()) {
		if (plan()->options() & BuildPlan::Simulate) {
			fout("rm -f %%\n") << path;
			return true;
		}
		fout("rm %%\n") << path;
		try {
			File::unlink(path);
		}
		catch (SystemError &ex) {
			fout("%%\n") << ex.what();
			return false;
		}
	}
	return true;
}

} // namespace fluxmake
