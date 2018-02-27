/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/regexp/RegExp>
#include <cc/regexp/Glob>

namespace cc {
namespace regexp {

Glob::Glob(String expression):
    remainder_(expression->split('/'))
{
    if (expression->head(1) == "/") {
        remainder_->pop(0);
        init("/");
    }
    else {
        init(".");
    }
}

Glob::Glob(String path, StringList *remainder):
    remainder_(remainder->copy())
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
            String node = dir_->path()->extendPath(name);
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

}} // namespace cc::regexp
