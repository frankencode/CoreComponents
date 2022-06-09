/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/AppBar>
#include <cc/Control>
#include <cc/Picture>
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
            .centerLeft([this]{
                return Point{
                    showNavButton * (sp(16) + sp(24)) + sp(32),
                    sp(28)
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

    void settled() override
    {
        /** TODO: logic for adding the overflow/context menu button
          */

        double right = sp(12);
        const double step = sp(48);
        for (const Action &action: actions_) {
            Control button;
            right += step;
            add(
                Control{}
                .associate(&button)
                .onClicked([action]{ action(); })
                .paper([this, button]{ return theme().appBarColor(button.pressed()); })
                .size(sp(48), sp(56))
                .pos([this,right]{ return Point{width() - right, 0}; })
                .add(
                    action.icon()
                    .color([this]{ return theme().appBarTextColor(); })
                    .centerInParent()
                )
            );
        }
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
                Picture{ideographic}
                .color([this]{ return theme().appBarTextColor(); })
                .centerLeft([]{ return Point{sp(16), sp(28)}; })
            )
        );

        showNavButton(true);

        return true;
    }

    void onNavigate(Fun<void()> &&f)
    {
        addNavButton(Ideographic::Menu);
        onNavButtonClicked(move(f));
    }

    void onDismissed(Fun<void()> &&f)
    {
        addNavButton(Ideographic::ArrowLeft);
        onNavButtonClicked(move(f));
    }

    Property<String> title;
    Property<bool> showNavButton { false };
    Trigger onNavButtonClicked;
    Control navButton_;
    List<Action> actions_;
};

double AppBar::height()
{
    return sp(56);
}

AppBar::AppBar():
    View{onDemand<State>}
{}

AppBar &AppBar::associate(Out<AppBar> self)
{
    return View::associate<AppBar>(self);
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
    me().title(move(f));
    return *this;
}

AppBar &AppBar::addAction(const Action &action)
{
    assert(!hasParent()); // Actions needs to be added before the AppBar is settled.
    me().actions_.append(action);
    return *this;
}

AppBar &AppBar::onNavigate(Fun<void()> &&f)
{
    me().onNavigate(move(f));
    return *this;
}

AppBar &AppBar::onDismissed(Fun<void()> &&f)
{
    me().onDismissed(move(f));
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
