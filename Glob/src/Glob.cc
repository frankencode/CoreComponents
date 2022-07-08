/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Glob>
#include <cc/List>
#include <cc/Dir>
#include <cc/Pattern>

namespace cc {

struct Glob::State: public Object::State
{
    explicit State(const String &text):
        remainder_{text.split('/')}
    {
        if (text.startsWith('/')) {
            remainder_.popFront();
            init("/");
        }
        else {
            init(".");
        }
    }

    State(const String &path, const List<String> &remainder):
        remainder_{remainder}
    {
        init(path);
    }

    void init(const String &path)
    {
        dir_ = Dir{path};
        String text = remainder_.first();
        if (!text.endsWith('^')) text = text + "^";
        pattern_ = text;
        remainder_.popFront();
    }

    bool read(Out<String> path)
    {
        if (child_) {
            if (child_.read(path))
                return true;
            child_.Object::me = nullptr;
        }
        for (String name; dir_.read(&name);) {
            if (name == ".") continue;
            if (name == ".." && pattern_.text() != "..") continue;
            if (pattern_.match(name)) {
                String node = dir_.path() / name;
                if (remainder_.count() == 0) {
                    path = node;
                    return true;
                }
                if (Dir::exists(node)) {
                    child_.Object::me = new State{node, remainder_};
                    return read(path);
                }
            }
        }
        return false;
    }

    Dir dir_;
    Pattern pattern_;
    List<String> remainder_;
    Glob child_;
};

Glob::Glob(const String &text):
    Object{new State{text}}
{}

bool Glob::read(Out<String> path)
{
    return me().read(path);
}

List<String> Glob::explain() const
{
    return me().pattern_.explain();
}

String Glob::text() const
{
    return me().pattern_.text();
}

Glob::State &Glob::me()
{
    return Object::me.as<State>();
}

const Glob::State &Glob::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
