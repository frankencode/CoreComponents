/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include "HeaderStyle.h"

namespace cc { template<class> class Singleton; }

namespace ccclaim {

using namespace cc;

class Registry: public Object
{
public:
    inline int headerStyleCount() const { return headerStyleByLanguage_->count(); }
    HeaderStyle *headerStyleAt(int i) const { return headerStyleByLanguage_->at(i)->value(); }

    const HeaderStyle *headerStyleByLanguage(String language) const;
    bool detectHeaderStyle(String path, String text, HeaderStyle **style) const;

private:
    friend class Singleton<Registry>;
    friend class HeaderStyle;

    Registry();

    void registerHeaderStyle(HeaderStyle *style);

    typedef Map< String, Ref<HeaderStyle> > HeaderStyleByLanguage;
    Ref<HeaderStyleByLanguage> headerStyleByLanguage_;
};

Registry *registry();

} // namespace ccclaim
