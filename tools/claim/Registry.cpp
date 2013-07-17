#include "Registry.h"

namespace fclaim
{

Registry::Registry()
	: headerScanners_(HeaderScannerList::create())
{}

const HeaderScannerList *Registry::headerScanners() const
{
	return headerScanners_;
}

void Registry::registerHeaderScanner(HeaderScanner *scanner)
{
	headerScanners_->append(scanner);
}

} // namespace fclaim
