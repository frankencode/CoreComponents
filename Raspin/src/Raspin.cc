/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Raspin>
#include <cc/File>

namespace cc {

struct Raspin::State final: public Singleton::State
{
    State():
        io_{File{"/dev/gpiomem", FileOpen::ReadWrite|FileOpen::Sync}.map(0, 0xF4)}
    {}

    Bytes io_;
};

Raspin::Raspin():
    Singleton{instance<State>()}
{}

Raspin::Pin Raspin::operator[](int index)
{
    return Pin{static_cast<uint32_t *>(me().io_), index};
}

Raspin::Group Raspin::select(std::initializer_list<int> pins)
{
    return Group{static_cast<uint32_t *>(me().io_), pins};
}

Raspin::State &Raspin::me()
{
    return Object::me.as<State>();
}

} // namespace cc
