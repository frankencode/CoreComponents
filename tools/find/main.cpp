/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Arguments.h>
#include <flux/Pattern.h>
#include <flux/DirWalker.h>
#include <flux/File.h>

using namespace flux;

class Match: public Object {
public:
	Match(int ln, int i0, int i1): ln_(ln), i0_(i0), i1_(i1) {}
	int ln_, i0_, i1_;
};

typedef List< Ref<Match> > Matches;

void displayMatch(ByteArray *path, ByteArray *text, Match *match);

String replaceMatches(ByteArray *text, Matches *matches, ByteArray *replacement);

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();

	try {
		Ref<Arguments> arguments = Arguments::parse(argc, argv);
		{
			Ref<VariantMap> prototype = VariantMap::create();
			prototype->insert("path", "");
			prototype->insert("name", "");
			prototype->insert("type", "");
			prototype->insert("depth", -1);
			prototype->insert("text", "");
			prototype->insert("word", "");
			prototype->insert("display", false);
			prototype->insert("replace", "");
			prototype->insert("paste", "");
			arguments->validate(prototype);
		}

		Ref<VariantMap> options = arguments->options();
		Ref<StringList> items = arguments->items();

		Pattern pathPattern;
		Pattern namePattern;
		Pattern typePattern;
		int maxDepth = options->value("depth", -1);
		Pattern textPattern;
		bool displayOption = options->value("display", false);
		bool replaceOption = false;
		String replacement;

		String h;
		if (options->lookup("path", &h)) pathPattern = h;
		if (options->lookup("name", &h)) namePattern = h;
		if (options->lookup("type", &h)) typePattern = h;
		if (options->lookup("text", &h)) textPattern = h;
		if (options->lookup("word", &h)) textPattern = String("(!<:[a..z]|[A..Z]|[0..9]|_)" + h + "(!>:[a..z]|[A..Z]|[0..9]|_)");

		if (options->lookup("replace", &h)) {
			replaceOption = true;
			replacement = h;
		}
		if (options->lookup("paste", &h)) {
			replaceOption = true;
			replacement = File::open(h)->map();
		}

		if (items->size() == 0) items->append(".");

		for (int i = 0; i < items->size(); ++i) {
			String dirPath = items->at(i)->canonicalPath();
			Ref<DirWalker> dirWalker = DirWalker::open(dirPath);
			dirWalker->setMaxDepth(maxDepth);
			String path;
			while (dirWalker->read(&path)) {
				if (pathPattern) {
					if (!pathPattern->match(path)->valid()) continue;
				}
				if (namePattern) {
					if (!namePattern->match(path->fileName())->valid()) continue;
				}
				if (typePattern) {
					int type = FileStatus::read(path, false)->type();
					bool shortMode = (typePattern->matchLength() == 1);
					String typeString;
					if (type == File::Regular)          typeString = shortMode ? "r" : "regular file";
					else if (type == File::Directory)   typeString = shortMode ? "d" : "directory";
					else if (type == File::Symlink)     typeString = shortMode ? "l" : "symlink";
					else if (type == File::CharDevice)  typeString = shortMode ? "c" : "character device";
					else if (type == File::BlockDevice) typeString = shortMode ? "b" : "block device";
					else if (type == File::Fifo)        typeString = shortMode ? "f" : "fifo";
					else if (type == File::Socket)      typeString = shortMode ? "s" : "socket";
					if (!typePattern->find(typeString)->valid()) continue;
				}
				if (textPattern) {
					if (FileStatus::read(path)->type() != File::Regular)
						continue;
					Ref<File> file = File::tryOpen(path, File::ReadOnly);
					if (!file) {
						ferr("Failed to open %%\n") << path;
						continue;
					}
					Ref<Matches> matches = Matches::create();
					String text = file->map();
					int ln = 1;
					for (int i = 0; i < text->size();) {
						Ref<Token> token = textPattern->find(text, i)->rootToken();
						if (!token) break;
						for (;i < token->i0(); ++i)
							if (text->at(i) == '\n') ++ln;
						matches->append(new Match(ln, token->i0(), token->i1()));
						for (;i < token->i1(); ++i)
							if (text->at(i) == '\n') ++ln;
						if (token->i0() == token->i1()) ++i;
					}
					if (replaceOption && matches->size() > 0) {
						file = File::tryOpen(path, File::ReadWrite);
						if (!file) {
							ferr("Failed to write %%\n") << path;
							continue;
						}
						text = replaceMatches(text, matches, replacement);
						file->truncate(0);
						file->write(text);
					}
					for (int i = 0; i < matches->size(); ++i) {
						Match *match = matches->at(i);
						if (displayOption)
							displayMatch(path, text, match);
						else
							fout("%%:%%:%%..%%\n") << path << match->ln_ << match->i0_ << match->i1_;
					}
					continue;
				}
				fout() << path << nl;
			}
		}
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [DIR]...\n"
			"Find (and replace) recursively descending into each [DIR]\n"
			"\n"
			"Options:\n"
			"  -path  file path pattern\n"
			"  -name  file name pattern\n"
			"  -type  file type pattern\n"
			"           r .. regular file\n"
			"           d .. directory\n"
			"           l .. symbolic link\n"
			"           c .. character device\n"
			"           b .. block device\n"
			"           f .. fifo\n"
			"           s .. socket\n"
			"  -depth  maximum depth of recursive directory search\n"
			"  -text  text search pattern\n"
			"  -word  word search pattern\n"
			"  -display  display match in context\n"
			"  -replace  replace matches by given text\n"
			"  -paste  paste replacements from file\n"
		) << toolName;
		return 1;
	}
	catch (Exception &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		return 1;
	}

	return 0;
}

void displayMatch(ByteArray *path, ByteArray *text, Match *match)
{
	int ln = match->ln_;
	int i0 = match->i0_;
	int i1 = match->i1_;

	int j0 = i0;
	for (;j0 > 0; --j0)
		if (text->at(j0 - 1) == '\n') break;

	fout("%%:") << path;
	if (i0 == i1) {
		fout("%%: %%\n") << ln << text->copy(j0, text->find("\n", i0));
		return;
	}

	bool multiline = text->copy(i0, i1)->contains('\n');
	if (multiline) fout() << nl;

	for (int j1 = j0; j0 < i1; j0 = j1) {
		for (;j1 < text->size(); ++j1)
			if (text->at(j1) == '\n') break;
		Format line;
		line << ln << ": ";
		int k0 = j0, k1 = j1;
		if (j0 <= i0 && i0 < j1) k0 = i0;
		if (j0 < i1 && i1 < j1) k1 = i1;
		if (j0 < k0) line << text->copy(j0, k0);
		if (k0 < k1) line << "\033[7m" << text->copy(k0, k1) << "\033[m";
		if (k1 < j1) line << text->copy(k1, j1);
		line << "\n";
		fout() << line->join();
		++ln;
		++j1;
	}
}

String replaceMatches(ByteArray *text, Matches *matches, ByteArray *replacement)
{
	Ref<StringList> fragments = StringList::create();
	int fi0 = 0; // begin of fragment
	int si = 0, sl = 0; // index and line shift
	int nr = replacement->count('\n');
	for (int i = 0; i < matches->size(); ++i) {
		Match *match = matches->at(i);
		fragments->append(text->copy(fi0, match->i0_));
		fi0 = match->i1_;
		int i0s = match->i0_ + si;
		si += replacement->size() - (match->i1_ - match->i0_);
		sl += nr - text->copy(match->i0_, match->i1_)->count('\n');
		match->i0_ = i0s;
		match->i1_ = i0s + replacement->size();
		match->ln_ += sl;
	}
	fragments->append(text->copy(fi0, text->size()));
	return fragments->join(replacement);
}
