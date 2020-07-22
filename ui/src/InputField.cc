/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/InputField>
#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/InputLine>
#include <cc/ui/Label>
#include <cc/ui/TextInput>
#include <cc/ui/Transition>

namespace cc {
namespace ui {

InputField::Instance::Instance(const String &labelText_):
    labelText{labelText_}
{
    paper <<[=]{
        return theme()->inputFieldFillColor();
            // FIXME: should be a function on background Color
            // FIXME: make depend on input field type
    };

    build >>[=]{
        InputControl input = createInputControl();
        input->pos = Point { dp(12), dp(24) };
        input->accepted >>[=]{ accepted(); };
        input->rejected >>[=]{ rejected(); };
        input->gotoNext >>[=]{ gotoNext(); };
        input->gotoPrevious >>[=]{ gotoPrevious(); };
        (*this) << input;
        input_ = input;
        inputText <<[=]{ return input_->text(); };

        size <<[=]{ return preferredSize(); };

        Label dummy;
        dummy->text <<[=]{ return dummyText(); };
        dummy->ink <<[=]{ return theme()->secondaryTextColor(); };
        dummy->visible <<[=]{ return dummy->text() && !input->text(); };
        dummy->pos <<[=]{
            return Point {
                input->left(),
                input->size()[1]/2 - dummy->size()[1] / 2
            };
        };

        (*this) << dummy;

        Label label;
        label->text <<[=]{ return labelText(); };
        label->ink <<[=]{
            return (pressed() || focus()) ?
                style()->theme()->focusTextColor() :
                style()->theme()->secondaryTextColor();
        };
        {
            Label smallLabel;
            smallLabel->text <<[=]{ return labelText(); };
            smallLabel->font <<[=]{ return Application{}->smallFont(); };
            smallLabel->pos <<[=]{ return Point { dp(12), dp(12) }; };
            smallLabel->visible = false;
            (*this) << smallLabel;

            Label bigLabel;
            bigLabel->text <<[=]{ return labelText(); };
            bigLabel->font <<[=]{ return Application{}->defaultFont(); };
            bigLabel->pos <<[=]{ return Point { dp(12), size()[1] / 2 - bigLabel->size()[1] / 2 }; };
            bigLabel->visible = false;
            (*this) << bigLabel;

            label->font <<[=]{
                return (focus() || input->text() || dummy->text()) ?
                    smallLabel->font() :
                    bigLabel->font();
            };
            label->pos <<[=]{
                return (focus() || input->text() || dummy->text()) ?
                    smallLabel->pos() :
                    bigLabel->pos();
            };

            auto easing = easing::Bezier(0.5, 0.0, 0.5, 1.0);
            easeOn(label->font, 0.1, easing);
            easeOn(label->pos, 0.1, easing);
        }
        (*this) << label;

        InputLine inputLine{dp(2)};
        inputLine->thickness <<[=]{ return (hover() || pressed() || focus()) ? dp(2) : dp(1); };
        inputLine->ink <<[=]{
            if (pressed()) return style()->theme()->pressedInputLineColor();
            if (focus()) return style()->theme()->focusInputLineColor();
            if (hover()) return style()->theme()->hoverInputLineColor();
            return style()->theme()->inputLineColor();
        };
        inputLine->pos <<[=]{
            return Point { 0, std::ceil(size()[1]) - inputLine->size()[1] };
        };

        (*this) << inputLine;

        delegate = input;
    };

    paint >>[=]{
        const double w = size()[0];
        const double h = size()[1];
        const double r = dp(6);

        Painter p{this};
        p->moveTo(Point{0, h});
        p->lineTo(Point{0, r});
        p->arc(Point{r, r}, r, degree(180), degree(270));
        p->lineTo(Point{w - r, 0});
        p->arc(Point{w - r, r}, r, degree(270), degree(360));
        p->lineTo(Point{w, h});
        p->setSource(paper());
        p->fill();
    };
}

Size InputField::Instance::preferredSize() const
{
    return input_->preferredSize() + Size { std::ceil(dp(24)), std::ceil(dp(32)) };
}

Size InputField::Instance::minSize() const
{
    return input_->minSize() + Size { std::ceil(dp(24)), std::ceil(dp(32)) };
}

Size InputField::Instance::maxSize() const
{
    return input_->maxSize() + Size { std::ceil(dp(24)), std::ceil(dp(32)) };
}

void InputField::Instance::clear()
{
    View::Instance::clear(parent()->paper());
}

void InputField::Instance::gotoNext()
{
    View p = parent();
    if (!p) return;

    bool findMyself = true;
    for (int i = 0; i < p->childCount(); ++i) {
        InputField inputField = p->childAt(i)->as<InputField>();
        if (inputField) {
            if (findMyself) {
                if (inputField == this)
                    findMyself = false;
            }
            else {
                Application{}->focusControl = inputField->input_;
                break;
            }
        }
    }
}

void InputField::Instance::gotoPrevious()
{
    View p = parent();
    if (!p) return;

    bool findMyself = true;
    for (int i = p->childCount() - 1; i >= 0; --i) {
        InputField inputField = p->childAt(i)->as<InputField>();
        if (inputField) {
            if (findMyself) {
                if (inputField == this)
                    findMyself = false;
            }
            else {
                Application{}->focusControl = inputField->input_;
                break;
            }
        }
    }
}

}} // namespace cc::ui
