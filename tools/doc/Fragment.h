#ifndef FLUXDOC_FRAGMENT_H
#define FLUXDOC_FRAGMENT_H

#include <flux/Yason.h>

namespace fluxdoc
{

using namespace flux;

typedef YasonObject Fragment;
typedef List< Ref<Fragment> > FragmentList;

} // namespace fluxdoc

#endif // FLUXDOC_FRAGMENT_H
