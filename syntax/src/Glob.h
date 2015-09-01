/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_GLOB_H
#define FLUXSYNTAX_GLOB_H

#include <flux/syntax/RegExp>

namespace flux {

class Dir;

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

} // namespace flux

#endif // FLUXSYNTAX_GLOB_H
