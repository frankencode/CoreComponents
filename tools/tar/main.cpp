/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/exceptions.h>
#include <flux/Format.h>
#include <flux/Arguments.h>
#include <flux/File.h>
#include <flux/TarReader.h>
#include <flux/ArReader.h>
#include <flux/TarWriter.h>
#include <flux/ArWriter.h>
#include "pack.h"
#include "unpack.h"

using namespace flux;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	bool tarMode = toolName->contains("tar");
	bool unpackMode = toolName->contains("un");
	try {
		Ref<Arguments> arguments = Arguments::parse(argc, argv);
		{
			Ref<VariantMap> prototype = VariantMap::create();
			if (unpackMode) {
				prototype->insert("list", false);
				prototype->insert("status", false);
			}
			else {
				prototype->insert("output", "");
			}
			prototype->insert("verbose", false);
			arguments->validate(prototype);
		}

		Ref<VariantMap> options = arguments->options();
		Ref<StringList> items = arguments->items();

		if (unpackMode) {
			if (items->size() == 0) items->append(String());

			for (int i = 0; i < items->size(); ++i) {
				String path = items->at(i);

				Ref<Stream> source;
				if (path != "") source = File::open(path);
				else source = in();

				Ref<ArchiveReader> archive;
				if (tarMode) archive = TarReader::open(source);
				else archive = ArReader::open(source);

				if (options->value("list")) fluxtar::list(archive);
				else if (options->value("status")) fluxtar::status(archive);
				else fluxtar::unpack(archive, options->value("verbose"));
			}
		}
		else {
			Ref<Stream> sink;
			String sinkPath = options->value("output");
			if (sinkPath != "") {
				try { File::unlink(sinkPath); } catch (SystemError &) {}
				File::create(sinkPath);
				sink = File::open(sinkPath, File::WriteOnly);
			}
			else {
				sink = out();
			}

			Ref<ArchiveWriter> archive;
			if (tarMode) archive = TarWriter::open(sink);
			else archive = ArWriter::open(sink);

			for (int i = 0; i < items->size(); ++i)
				fluxtar::pack(items->at(i), archive, options->value("verbose"));
		}
	}
	catch (HelpError &) {
		if (unpackMode) {
			fout(
				"Usage: %% [OPTION]... [FILE]...\n"
				"Unpack or list contents of archive files.\n"
				"\n"
				"Options:\n"
				"  -list     list contents\n"
				"  -status   list archived file status\n"
				"  -verbose  verbose output\n"
			) << toolName;
		}
		else {
			fout(
				"Usage: %% [DIR]...\n"
				"Pack an archive file.\n"
				"\n"
				"Options:\n"
				"  -output   archive file to create\n"
				"  -verbose  verbose output\n"
			) << toolName;
		}
		return 1;
	}
	catch (BrokenArchive &ex) {
		ferr() << toolName << ": Broken archive: " << ex.reason() << " (file offset 0x" << hex(ex.offset()) << ")" << nl;
		return 1;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}

	return 0;
}
