/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/regexp/RegExp>

namespace cc { class Dir; }

namespace cc {
namespace regexp {

/** \brief Collect file names matching a globbing pattern
  * \see DirWalker
  */
class Glob: public Source<String>
{
public:
    inline static Ref<Glob> open(String expression) { return new Glob(expression); }

    bool read(String *path);

private:
    Glob(String expression);
    Glob(String path, StringList *remainder);
    void init(String path);

    Ref<Dir> dir_;
    RegExp pattern_;
    Ref<StringList> remainder_;
    Ref<Glob> child_;
};

}} // namespace cc::regexp

