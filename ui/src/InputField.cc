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
#include <cc/ui/Label>
#include <cc/ui/TextInput>
#include <cc/ui/Easing>

namespace cc {
namespace ui {

InputField::Instance::Instance(const InputControl &input, const String &inputLabel):
    labelText{inputLabel},
    input_{input}
{
    delegate = input_;

    paper <<[=]{
        return theme()->inputFieldFillColor();
            // FIXME: should be a function on background Color
            // FIXME: make depend on input field type
    };

    input_->pos = Point { dp(12), dp(24) };
    input_->accepted >>[=]{ accepted(); };
    input_->rejected >>[=]{ rejected(); };
    input_->gotoNext >>[=]{ gotoNext(); };
    input_->gotoPrevious >>[=]{ gotoPrevious(); };
    (*this) << input_;

    inputText <<[=]{ return input_->text(); };

    size <<[=]{ return preferredSize(); };

    Label dummy;
    dummy->text <<[=]{ return dummyText(); };
    dummy->ink <<[=]{ return theme()->secondaryTextColor(); };
    dummy->visible <<[=]{ return dummy->text() && !input_->text(); };
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

        auto easing = Easing::Bezier(0.5, 0.0, 0.5, 1.0);
        Easing{label->font, 0.1, easing};
        Easing{label->pos, 0.1, easing};
    }
    (*this) << label;

    {
        View underline;
        underline->inheritPaper();
        underline->size <<[=]{
            return Size{size()[0], std::ceil(dp(2))};
        };
        underline->pos <<[=]{
            return Point { 0, std::ceil(size()[1]) - underline->size()[1] };
        };
        underlineThickness <<[=]{ return (hover() || pressed() || focus()) ? dp(2) : dp(1); };
        underlineInk <<[=]{
            if (pressed()) return style()->theme()->pressedInputLineColor();
            if (focus()) return style()->theme()->focusInputLineColor();
            if (hover()) return style()->theme()->hoverInputLineColor();
            return style()->theme()->inputLineColor();
        };
        underline->paint <<[=]{
            Painter p{underline};
            p->rectangle(Point{ 0, underline->size()[1] - underlineThickness() }, underline->size());
            p->setSource(underlineInk());
            p->fill();
        };
        (*this) << underline;
    }

    paint <<[=]{
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

#if 0
void InputField::Instance::clear()
{
    if (parent()) View::Instance::clear(parent()->paper()); // FIXME, why not use inheritPaper()?!
}
#endif

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
