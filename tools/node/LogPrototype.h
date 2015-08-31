/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_LOGPROTOTYPE_H
#define FLUXNODE_LOGPROTOTYPE_H

#include <flux/Date>
#include <flux/meta/MetaObject>

namespace fluxnode {

using namespace flux;

class LogPrototype: public MetaObject
{
public:
    static Ref<LogPrototype> create() { return new LogPrototype; }

private:
    LogPrototype()
        : MetaObject("Log")
    {
        insert("path", "");
        insert("level", "");
        insert("retention", days(30));
        insert("rotation", days(1));
    }
};

} // namespace fluxnode

#endif // FLUXNODE_LOGPROTOTYPE_H
