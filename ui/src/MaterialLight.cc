/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/colors>
#include <cc/ui/MaterialLight>

namespace cc {
namespace ui {

MaterialLight::MaterialLight()
{
    primaryColor = Material::Purple500;
    accentColor = Material::Cyan500;
    alertColor = Material::RedA400;

    statusBarColor = Material::Grey300;
    appBarColor = Material::Grey100;
    backgroundColor = Material::Grey50;
    dialogBackgroundColor = Material::White;
}

}} // namespace cc::ui
