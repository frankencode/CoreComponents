/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimePicker>
#include <cc/Text>
#include <cc/RowLayout>
#include <cc/NumberCell>
#include <cc/Checkbox>
#include <cc/Label>
#include <cc/ClockFace>
#include <cc/TextButton>
#include <cc/Window>
#include <cc/Timezone>

namespace cc {

struct TimePicker::State final: public Dialog::State
{
    State(int initialHour = 12, int initialMinute = 0):
        hour{initialHour},
        minute{initialMinute}
    {
        add(
            Text{
                String{"Select time"}.upcased(),
                style().defaultSmallFont()
            }
            .associate(&title_)
            .baselineStart([this]{
                return Point{ sp(24), sp(28) };
            })
            .paper([this]{
                return theme().dialogColor();
            })
        );

        add(
            View{}
            .associate(&inputArea_)
            .topCenter([this]{
                return Point{width() / 2, title_.baselineStart()[1] + sp(16)};
            })
            .add(
                NumberCell{}
                .associate(&hourInput_)
                .font([this]{
                    Font font = style().defaultFont();
                    font.size(sp(50));
                    return font;
                })
                .textColor([this]{
                    return
                        clockMode() == ClockFace::Mode::SetHour && !textInput() ?
                        theme().clockDialCurrentValueTextColor(false) :
                        theme().primaryTextColor();
                })
                .color([this]{
                    return
                        clockMode() == ClockFace::Mode::SetHour && !textInput() ?
                        theme().clockDialCurrentValueColor(false) :
                        theme().inputFieldFillColor(hourInput_.pressed());
                })
                .digits(2)
                .min(0)
                .max(23)
                .value(initialHour)
                .textInput([this]{
                    return textInput();
                })
                /*.onRejected([this]{ // FIXME
                    onRejected();
                })*/
                .onClicked([this]{
                    clockMode(ClockFace::Mode::SetHour);
                })
                .attach([this]{
                    hour(static_cast<int>(hourInput_.value()));
                })
                /*.attach([this]{
                    CC_INSPECT(hourInput_.size());
                })*/
            )
            .add(
                Label{":"}
                .associate(&colonLabel_)
                .font([this]{ return hourInput_.font(); })
                .paper([this]{ return theme().dialogColor(); })
                .padding(
                    Padding{}
                    .left(sp(-5))
                    .right(sp(-5))
                )
            )
            .add(
                NumberCell{}
                .associate(&minuteInput_)
                .font([this]{ return hourInput_.font(); })
                .textColor([this]{
                    return
                        clockMode() == ClockFace::Mode::SetMinute && !textInput() ?
                        theme().clockDialCurrentValueTextColor(false) :
                        theme().primaryTextColor();
                })
                .color([this]{
                    return
                        clockMode() == ClockFace::Mode::SetMinute && !textInput() ?
                        theme().clockDialCurrentValueColor(false) :
                        theme().inputFieldFillColor(minuteInput_.pressed());
                })
                .digits(2)
                .min(0)
                .max(59)
                .value(initialMinute)
                .textInput([this]{
                    return textInput();
                })
                /*.onAccepted([this]{
                    onAccepted();
                })*/
                .onClicked([this]{
                    clockMode(ClockFace::Mode::SetMinute);
                })
                .attach([this]{
                    minute(static_cast<int>(minuteInput_.value()));
                })
            )
            .layout(
                RowLayout{}
                .align(RowAlign::Center)
                .spacing(0)
                .margin(0)
            )
        );

        add(
            View{}
            .associate(&dialArea_)
            .topCenter([this]{
                return inputArea_.bottomCenter();
            })
            .size([this]{
                return Size{
                    width(),
                    width()
                };
            })
            .visible([this]{
                return !textInput();
            })
            .add(
                ClockFace{}
                .associate(&clockFace_)
                .mode([this]{ return clockMode(); })
                .hour(initialHour)
                .minute(initialMinute)
                .onHourChanged([this]{
                    hourInput_.value(clockFace_.hour());
                })
                .onMinuteChanged([this]{
                    minuteInput_.value(clockFace_.minute());
                })
                .margin(Size{sp(24), sp(24)})
                .size([this]{
                    return dialArea_.size();
                })
            )
            #if 0
            .attach([this]{
                CC_INSPECT(dialArea_.pos());
                CC_INSPECT(dialArea_.size());
                CC_INSPECT(clockFace_.size());
            })
            #endif
        );

        add(
            TextButton{
                Picture{Icon::ClockOutline, sp(18)}
            }
            .onTriggered([this]{
                textInput(false);
            })
            .bottomLeft([this]{
                return Point{sp(8), height() - sp(8)};
            })
            .paper([this]{
                return theme().dialogColor();
            })
            .padding(sp(8))
            .visible([this]{
                return textInput();
            })
        );

        add(
            TextButton{
                Picture{Icon::KeyboardOutline, sp(18)}
            }
            .onTriggered([this]{
                textInput(true);
                hourInput_.focus(true);
            })
            .bottomLeft([this]{
                return Point{sp(8), height() - sp(8)};
            })
            .paper([this]{
                return theme().dialogColor();
            })
            .padding(sp(8))
            .visible([this]{
                return !textInput();
            })
        );

        addAction(
            Action{"AM / PM"}
            .onTriggered([this]{
                clockFace_.pm(!clockFace_.pm());
            })
        );

        addAction(
            Action{"Cancel"}
            ([this]{
                onRejected();
                close();
            })
        );

        addAction(
            Action{"OK"}
            ([this]{
                onAccepted();
                close();
            })
        );

        size([this]{ return preferredSize(); });

        attach([this]{ // FIXME: why needed?
            if (!hasParent() && hasWindow()) {
                window().size(getSize());
            }
        });
    }

    Size getSize() const
    {
        return Size{sp(300), inputArea_.bottom() + (textInput() ? 0 : (dialArea_.height() - sp(8))) + buttonArea().height()};
    }

    Size minSize() const override
    {
        return getSize();
    }

    Size maxSize() const override
    {
        return getSize();
    }

    Size preferredSize() const override
    {
        return getSize();
    }

    void setHour(int newValue)
    {
        hour(newValue);
        hourInput_.value(newValue);
        clockFace_.hour(newValue);
    }

    void setMinute(int newValue)
    {
        minute(newValue);
        minuteInput_.value(newValue);
        clockFace_.minute(newValue);
    }

    Text title_;
    View inputArea_;
    NumberCell hourInput_;
    NumberCell minuteInput_;
    Label colonLabel_;
    View dialArea_;
    ClockFace clockFace_;
    Property<ClockFace::Mode> clockMode { ClockFace::Mode::SetHour };
    Property<int> hour;
    Property<int> minute;
    Property<bool> textInput { false };
    Trigger onAccepted;
    Trigger onRejected;
};

TimePicker::TimePicker():
    Dialog{onDemand<State>}
{}

TimePicker::TimePicker(int hour, int minute):
    Dialog{new State{hour, minute}}
{}

TimePicker &TimePicker::associate(Out<TimePicker> self)
{
    return View::associate<TimePicker>(self);
}

TimePicker &TimePicker::hour(double newValue)
{
    me().setHour(newValue);
    return *this;
}

int TimePicker::hour() const
{
    return me().hour();
}

TimePicker &TimePicker::minute(double newValue)
{
    me().setMinute(newValue);
    return *this;
}

int TimePicker::minute() const
{
    return me().minute();
}

TimePicker &TimePicker::onAccepted(Function<void()> &&f)
{
    me().onAccepted(move(f));
    return *this;
}

TimePicker &TimePicker::onRejected(Function<void()> &&f)
{
    me().onRejected(move(f));
    return *this;
}

TimePicker::State &TimePicker::me()
{
    return get<State>();
}

const TimePicker::State &TimePicker::me() const
{
    return get<State>();
}

} // namespace cc
