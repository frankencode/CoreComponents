/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/regexp/RegExp>
#include <cc/Source>

namespace cc { class Dir; }

namespace cc {
namespace regexp {

/** \brief Collect file names matching a globbing pattern
  * \see DirWalker
  */
class Glob: public Source<String>
{
public:
    /** Create a new globbing source
      * \param expression globbing expression to use for path matching
      * \return new object instance
      */
    inline static Ref<Glob> open(String expression) { return new Glob(expression); }

    /** Get the next matched path
      * \param path returns the next path
      * \return true if another mathing path could be read
      */
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

CC_DEFINE_SOURCE_BEGIN_END(Glob);

}} // namespace cc::regexp
