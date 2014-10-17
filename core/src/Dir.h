/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_DIR_H
#define FLUX_DIR_H

#include <sys/types.h> // mode_t
#include <dirent.h> // DIR, opendir, closedir, readdir_r
#include <flux/generics>
#include <flux/String>

namespace flux {

class Dir: public Source<String>
{
public:
    inline static Ref<Dir> open(String path) { return new Dir(path); }
    static Ref<Dir> tryOpen(String path);

    String path() const;
    String path(String name) const;
    bool read(String *name);

    static bool access(String path, int flags);
    static bool exists(String path);
    static int count(String path);
    static void create(String path, int mode = 0755);
    static void establish(String path, int mode = 0755);
    static void unlink(String path);

protected:
    Dir(String path, DIR *dir = 0);
    ~Dir();

    String path_;
    DIR *dir_;
};

} // namespace flux

#endif // FLUX_DIR_H
