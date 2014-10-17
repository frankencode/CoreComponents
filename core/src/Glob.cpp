/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Dir>
#include <flux/Pattern>
#include <flux/Glob>

namespace flux {

Glob::Glob(String expression)
    : remainder_(expression->split('/'))
{
    if (expression->head(1) == "/") {
        remainder_->pop(0);
        init("/");
    }
    else {
        init(".");
    }
}

Glob::Glob(String path, StringList *remainder)
    : remainder_(StringList::clone(remainder))
{
    init(path);
}

void Glob::init(String path)
{
    dir_ = Dir::open(path);
    pattern_ = remainder_->pop(0);
}

bool Glob::read(String *path)
{
    if (child_) {
        if (child_->read(path))
            return true;
        child_ = 0;
    }
    String name;
    while (dir_->read(&name)) {
        if (name == ".") continue;
        if ((name == "..") && (pattern_ != "..")) continue;
        if (pattern_->match(name)->valid()) {
            String node = dir_->path(name);
            if (remainder_->count() == 0) {
                *path = node;
                return true;
            }
            if (Dir::exists(node)) {
                child_ = new Glob(node, remainder_);
                return read(path);
            }
        }
    }
    return false;
}

} // namespace flux
