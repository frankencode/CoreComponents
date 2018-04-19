/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/colors>
#include <cc/ui/MaterialDark>

namespace cc {
namespace ui {

MaterialDark::MaterialDark()
{
    primaryColor = Material::Purple500;
    accentColor = Material::Cyan500;
    alertColor = Material::RedA400;

    statusBarColor = Material::Black;
    appBarColor = Material::Grey900;
    backgroundColor = Color { 0x303030 };
    dialogBackgroundColor = Material::Grey800;
}

}} // namespace cc::ui
