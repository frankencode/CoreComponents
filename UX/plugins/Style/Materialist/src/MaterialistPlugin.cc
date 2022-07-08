/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MaterialistPlugin>
#include <cc/FontManager>
#include <cc/MaterialLight>
#include <cc/GlyphVisual>
#include <cc/File>
#include <cc/Dir>
#include <cc/Bundle>
#include <cc/Registration>
#include <cc/DEBUG>

namespace cc {

struct MaterialistPlugin::State: public StylePlugin::State
{
    State():
        StylePlugin::State{"Industrial"}
    {
        theme_ = MaterialLight{};
    }

    void activate() override
    {
        #if 0
        if (File::exists("/usr/share/fonts/TTF/Roboto-Regular.ttf")) {
            // Arch
            FontManager{}.addPath("/usr/share/fonts/TTF", "Roboto");
            defaultFont_ = Font{"Roboto", sp(16)};
            defaultFixedFont_ = Font{"Roboto Mono", sp(16)};
        }
        else if (File::exists("/usr/share/fonts/truetype/roboto/unhinted/RobotoCondensed-Regular.ttf")) {
            // Debian
            FontManager{}.addPath("/usr/share/fonts/truetype/roboto/unhinted", "RobotoCondensed");
            defaultFont_ = Font{"RobotoCondensed", sp(16)};
            defaultFixedFont_ = Font{"Roboto Mono", sp(16)};
        }
        #endif

        if (!defaultFont_) {
            const char *notoPaths[] = {
                "/usr/share/fonts/noto", // Arch
                "/usr/share/fonts/truetype/noto" // Debian
            };

            for (auto path: notoPaths) {
                if (Dir::exists(path)) {
                    FontManager{}.addPath(path);
                    defaultFont_ = Font{"Noto Sans", sp(16)};
                    if (!defaultFixedFont_) defaultFixedFont_ = Font{"Noto Mono", sp(16)};
                    break;
                }
            }

            const char *notoCjkPaths[] = {
                "/usr/share/fonts/noto-cjk", // Arch
                "/usr/share/fonts/opentype/noto" // Debian
            };

            for (auto path: notoCjkPaths) {
                if (Dir::exists(path)) {
                    FontManager{}.addPath(path);
                }
            }
        }

        if (File::exists("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
            // Arch
            FontManager{}.addPath("/usr/share/fonts/TTF", "DejaVu");
            if (!defaultFont_) defaultFont_ = Font{"DejaVu Sans", sp(16)};
            if (!defaultFixedFont_) defaultFixedFont_ = Font{"DejaVu Sans Mono", sp(16)};
        }
        else if (File::exists("/usr/share/fonts/truetype/dejavu")) {
            // Debian
            String dejavuPath = "/usr/share/fonts/truetype/dejavu";
            if (Dir::exists(dejavuPath)) {
                FontManager{}.addPath(dejavuPath);
                if (!defaultFont_) defaultFont_ = Font{"DejaVu Sans", sp(16)};
                if (!defaultFixedFont_) defaultFixedFont_ = Font{"DejaVu Sans Mono", sp(16)};
            }
        }

        if (!defaultFont_) CC_DEBUG << "Failed to locate default font";

        defaultSmallFont_ = defaultFont_;
        defaultSmallFont_.size(sp(12));

        defaultMediumFont_ = defaultFont_;
        defaultMediumFont_.size(sp(14));

        String iconsPath = CC_BUNDLE_LOOKUP("icons");
        if (iconsPath != "")
            FontManager{}.addPath(iconsPath);
        else
            CC_DEBUG << "Failed to locate icons directory";

        #ifndef NDEBUG
        CC_INSPECT(defaultFont_.path());
        CC_INSPECT(defaultFixedFont_.path());
        #endif
    }

    Visual icon(Icon ch, double size) const override
    {
        if (size <= 0)
            return GlyphVisual{+ch, iconFont_};

        Font font = iconFont_;
        font.size(size);
        return GlyphVisual{+ch, font};
    }

    Font iconFont_{"Icons", sp(24)};
};

MaterialistPlugin::MaterialistPlugin():
    StylePlugin{new State}
{}

CC_REGISTRATION(MaterialistPlugin);

} // namespace cc
