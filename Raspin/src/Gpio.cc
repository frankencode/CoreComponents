/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/rpi/Gpio>
#include <cc/File>

namespace cc::rpi {

struct Gpio::State final: public Singleton::State
{
    State():
        io_{File{"/dev/gpiomem", FileOpen::ReadWrite|FileOpen::Sync}.map(0, 0xF4)}
    {}

    Bytes io_;
};

Gpio::Gpio():
    Singleton{instance<State>()}
{}

Gpio::Pin Gpio::operator[](int index)
{
    return Pin{static_cast<uint32_t *>(me().io_), index};
}

Gpio::Group Gpio::select(std::initializer_list<int> pins)
{
    return Group{static_cast<uint32_t *>(me().io_), pins};
}

Gpio::State &Gpio::me()
{
    return Object::me.as<State>();
}

} // namespace cc::rpi
