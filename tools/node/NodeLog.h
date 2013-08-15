/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_NODELOG_H
#define FNODE_NODELOG_H

#include <fkit/Singleton.h>
#include <fkit/Format.h>

namespace fnode
{

using namespace fkit;

class NodeLog: public Object, public Singleton<NodeLog>
{
public:
	enum LogLevel {
		SilentLevel  = 0,
		ErrorLevel   = 1,
		WarningLevel = 2,
		StatusLevel  = 3,
		SummaryLevel = 4,
		DebugLevel   = 5
	};

	void init(int logLevel);

	inline int logLevel() const { return logLevel_; }

	Stream *errorStream() const;
	Stream *warningStream() const;
	Stream *statusStream() const;
	Stream *summaryStream() const;
	Stream *debugStream() const;

private:
	friend class Singleton<NodeLog>;

	NodeLog();

	int logLevel_;
	Ref<Stream> errorStream_;
	Ref<Stream> warningStream_;
	Ref<Stream> statusStream_;
	Ref<Stream> summaryStream_;
	Ref<Stream> debugStream_;
};

inline NodeLog *nodeLog() { return NodeLog::instance(); }
inline Format error() { return Format(nodeLog()->errorStream()); }
inline Format warning() { return Format(nodeLog()->warningStream()); }
inline Format status() { return Format(nodeLog()->statusStream()); }
inline Format summary() { return Format(nodeLog()->summaryStream()); }
inline Format debug() { return Format(nodeLog()->debugStream()); }

} // namespace fnode

#endif // FNODE_NODELOG_H
