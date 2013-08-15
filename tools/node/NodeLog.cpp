#include <fkit/NullStream.h>
#include <fkit/stdio.h>
#include "NodeLog.h"

namespace fnode
{

NodeLog::NodeLog()
{}

void NodeLog::init(int logLevel)
{
	logLevel_ = logLevel;
	errorStream_   = (logLevel >= ErrorLevel)   ? err() : nullStream();
	warningStream_ = (logLevel >= WarningLevel) ? err() : nullStream();
	statusStream_  = (logLevel >= StatusLevel)  ? err() : nullStream();
	summaryStream_ = (logLevel >= SummaryLevel) ? err() : nullStream();
	debugStream_   = (logLevel >= DebugLevel)   ? err() : nullStream();
}

Stream *NodeLog::errorStream() const { return errorStream_; }
Stream *NodeLog::warningStream() const { return warningStream_; }
Stream *NodeLog::statusStream() const { return statusStream_; }
Stream *NodeLog::summaryStream() const { return summaryStream_; }
Stream *NodeLog::debugStream() const { return debugStream_; }

} // namespace fnode
