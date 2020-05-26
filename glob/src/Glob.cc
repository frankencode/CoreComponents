/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/glob/Pattern>
#include <cc/glob/Glob>

namespace cc {
namespace glob {

Ref<Glob> Glob::open(const string &expression)
{
    return new Glob{expression};
}

Glob::Glob(const string &expression):
    remainder_{expression->split('/')}
{
    if (expression->head(1) == "/") {
        remainder_->removeAt(0);
        init("/");
    }
    else {
        init(".");
    }
}

Glob::Glob(const string &path, StringList *remainder):
    remainder_(remainder->copy())
{
    init(path);
}

void Glob::init(const string &path)
{
    dir_ = Dir::open(path);
    pattern_ = remainder_->front();
    remainder_->popFront();
}

bool Glob::read(string *path)
{
    if (child_) {
        if (child_->read(path))
            return true;
        child_ = 0;
    }
    string name;
    while (dir_->read(&name)) {
        if (name == ".") continue;
        if ((name == "..") && (pattern_ != "..")) continue;
        if (pattern_->match(name)->valid()) {
            string node = dir_->path()->extendPath(name);
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

}} // namespace cc::glob
