/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/AppBar>
#include <cc/Control>
#include <cc/Icon>
#include <cc/Label>
#include <cc/Shadow>

namespace cc {

struct AppBar::State final: public View::State
{
    State()
    {
        paper([this]{ return theme().appBarColor(); });

        add(
            Label{}
            .text([this]{ return title(); })
            .font(sp(20))
            .color([this]{ return theme().appBarTextColor(); })
            .baselineStart([this]{
                return Point{
                    showNavButton * (sp(16) + sp(24)) + sp(32),
                    height() - sp(20)
                };
            })
        );

        decoration(
            Shadow{}
            .color([this]{ return theme().appBarShadowColor(); })
            .blurRadius(5)
        );

        size([this]{ return preferredSize(); });
    }

    Size preferredSize() const override
    {
        return Size{
            hasParent() ? parent().width() : sp(360),
            sp(56)
        };
    }

    //! \todo min and max size

    bool addNavButton(Ideographic ideographic)
    {
        if (navButton_) return false;

        add(
            navButton_
            .onClicked([this]{ onNavButtonClicked(); })
            .size(sp(56), sp(56))
            .paper([this]{ return theme().appBarColor(navButton_.pressed()); })
            .visible([this]{ return showNavButton(); })
            .add(
                Icon{ideographic}
                .color([this]{ return theme().appBarTextColor(); })
                // .pos([this]{ return Point{sp(16), sp(16)}; })
                .centerLeft([]{ return Point{sp(16), sp(28)}; })
            )
        );

        showNavButton(true);

        return true;
    }

    void onNavigate(Call<void()> &&f)
    {
        addNavButton(Ideographic::Menu);
        onNavButtonClicked(std::move(f));
    }

    void onDismissed(Call<void()> &&f)
    {
        addNavButton(Ideographic::ArrowLeft);
        onNavButtonClicked(std::move(f));
    }

    Property<String> title;
    Property<bool> showNavButton { false };
    Trigger onNavButtonClicked;
    Control navButton_;
};

AppBar::AppBar():
    View{onDemand<State>}
{}

AppBar::AppBar(Out<AppBar> self):
    View{new State}
{
    self = *this;
}

String AppBar::title() const
{
    return me().title();
}

AppBar &AppBar::title(const String &newValue)
{
    me().title(newValue);
    return *this;
}

AppBar &AppBar::title(Definition<String> &&f)
{
    me().title(std::move(f));
    return *this;
}

AppBar &AppBar::onNavigate(Call<void()> &&f)
{
    me().onNavigate(std::move(f));
    return *this;
}

AppBar &AppBar::onDismissed(Call<void()> &&f)
{
    me().onDismissed(std::move(f));
    return *this;
}

AppBar::State &AppBar::me()
{
    return View::me().as<State>();
}

const AppBar::State &AppBar::me() const
{
    return View::me().as<State>();
}

} // namespace cc
