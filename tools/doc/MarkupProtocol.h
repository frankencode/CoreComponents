#ifndef FLUXDOC_MARKUPPROTOCOL_H
#define FLUXDOC_MARKUPPROTOCOL_H

#include <flux/Yason.h>
#include <flux/Singleton.h>

namespace fluxdoc
{

using namespace flux;

class MarkupProtocol: public YasonProtocol, public Singleton<MarkupProtocol>
{
private:
	friend class Singleton<MarkupProtocol>;
	MarkupProtocol();
};

inline MarkupProtocol *markupProtocol() { return MarkupProtocol::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_MARKUPPROTOCOL_H
