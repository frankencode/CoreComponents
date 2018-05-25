/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/Column>
#include <cc/ui/InputLine>
#include <cc/ui/Label>
#include <cc/ui/TextInput>
#include <cc/ui/Transition>
#include <cc/ui/InputField>

namespace cc {
namespace ui {

InputField::InputField(View *parent, const String &labelText_):
    Control(parent),
    labelText(labelText_)
{
    paper->bind([=]{
        return theme()->inputFieldFillColor();
            // FIXME: should be a function on background color
            // FIXME: make depend on input field type
    });
}

void InputField::init()
{
    InputControl *input = addInputControl();
    input->pos = Point { dp(12), dp(24) };
    input->accepted->connect([=]{ accepted(); });
    input->rejected->connect([=]{ rejected(); });
    input_ = input;

    size->bind([=]{ return preferredSize(); });

    Label *dummy = add<Label>();
    dummy->text->bind([=]{ return dummyText(); });
    dummy->ink->bind([=]{ return theme()->secondaryTextColor(); });
    dummy->visible->bind([=]{ return dummy->text() && !input->text(); });
    dummy->pos->bind([=]{
        return Point {
            input->left(),
            input->size()[1]/2 - dummy->size()[1] / 2
        };
    });

    Label *label = add<Label>();
    label->text->bind([=]{ return labelText(); });
    label->ink->bind([=]{
        return (pressed() || focus()) ?
            style()->theme()->focusTextColor() :
            style()->theme()->secondaryTextColor();
    });
    {
        Label *smallLabel = add<Label>();
        smallLabel->text->bind([=]{ return labelText(); });
        smallLabel->font->bind([=]{ return app()->smallFont(); });
        smallLabel->pos->bind([=]{ return Point { dp(12), dp(12) }; });
        smallLabel->visible = false;

        Label *bigLabel = add<Label>();
        bigLabel->text->bind([=]{ return labelText(); });
        bigLabel->font->bind([=]{ return app()->defaultFont(); });
        bigLabel->pos->bind([=]{ return Point { dp(12), size()[1] / 2 - bigLabel->size()[1] / 2 }; });
        bigLabel->visible = false;

        label->font->bind([=]{
            return (focus() || input->text() || dummy->text()) ?
                smallLabel->font() :
                bigLabel->font();
        });
        label->pos->bind([=]{
            return (focus() || input->text() || dummy->text()) ?
                smallLabel->pos() :
                bigLabel->pos();
        });

        auto easing = easing::Bezier(0.5, 0.0, 0.5, 1.0);
        easeOn(label->font, 0.1, easing);
        easeOn(label->pos, 0.1, easing);
    }

    InputLine *inputLine = add<InputLine>(dp(2));
    inputLine->thickness->bind([=]{ return (hover() || pressed() || focus()) ? dp(2) : dp(1); });
    inputLine->ink->bind([=]{
        if (pressed()) return style()->theme()->pressedInputLineColor();
        if (focus()) return style()->theme()->focusInputLineColor();
        if (hover()) return style()->theme()->hoverInputLineColor();
        return style()->theme()->inputLineColor();
    });

    inputLine->pos->bind([=]{
        return Point { 0, std::ceil(size()[1]) - inputLine->size()[1] };
    });

    // inputLine->anchorBottomLeftTo(bottomLeft());

    delegate = input;

    #if 0
    View *messageArea = add<View>();
    messageArea->size->bind([=]{ return Size { size()[0], sp(12) }; });
    messageArea->padding = dp(8);

    Label *help = messageArea->add<Label>();
    help->font->bind([=]{ return app()->smallFont(); });
    help->ink->bind([=]{ return style()->theme()->secondaryTextColor(); });
    help->text->bind([=]{ return helpText(); });

    Label *error = messageArea->add<Label>();
    error->font->bind([=]{ return app()->smallFont(); });
    error->ink->bind([=]{ return style()->theme()->alertColor(); });
    error->text->bind([=]{ return "Error: " + errorText(); });

    Label *status = messageArea->add<Label>();
    status->font->bind([=]{ return app()->smallFont(); });
    status->text->bind([=]{ return statusText(); });
    status->pos->bind([=]{ return Point { messageArea->size()[0] - status->size()[0], 0 }; });

    help->visible->bind([=]{ return helpText() != "" && !error->visible() && input->visible(); } );
    status->visible->bind([=]{ return statusText() != "" && !error->visible() && focus(); });
    error->visible->bind([=]{ return errorText() != "" && focus(); });
    messageArea->visible->bind([=]{ return help->visible() || error->visible() || status->visible(); });
    #endif
}

Size InputField::preferredSize() const
{
    return input_->preferredSize() + Size { std::ceil(24), std::ceil(dp(32)) };
}

Size InputField::minSize() const
{
    return input_->minSize() + Size { std::ceil(24), std::ceil(dp(32)) };
}

Size InputField::maxSize() const
{
    return input_->maxSize() + Size { std::ceil(24), std::ceil(dp(32)) };
}

void InputField::clear()
{
    View::clear(parent()->paper());
}

void InputField::paint()
{
    const double w = size()[0];
    const double h = size()[1];
    const double r = dp(6);

    Painter p(this);
    p->moveTo(Point{0, h});
    p->lineTo(Point{0, r});
    p->arc(Point{r, r}, r, degree(180), degree(270));
    p->lineTo(Point{w - r, 0});
    p->arc(Point{w - r, r}, r, degree(270), degree(360));
    p->lineTo(Point{w, h});
    p->setSource(paper());
    p->fill();
}

}} // namespace cc::ui
