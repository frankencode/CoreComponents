/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/List>
#include <cc/ui/Application>
#include <cc/ui/Window>
#include <cc/ui/Image>
#include <cc/ui/View>

namespace cc {
namespace ui {

View::View(View *parent):
    serial_(0),
    window_(0),
    parent_(parent),
    children_(Children::create())
{
    if (parent)
        parent->insertChild(this);

    if (parent_) {
        pos->connect([=]{
            if (visible()) {
                update(
                    UpdateRequest::create(UpdateReason::Moved, this)
                );
            }
        });
    }

    size->connect([=]{
        if (visible()) {
            update(
                UpdateRequest::create(UpdateReason::Resized, this)
            );
        }
    });

    color->connect([=]{
        if (visible()) {
            update(
                UpdateRequest::create(UpdateReason::Changed, this)
            );
        }
    });

    visible->connect([=]{
        if (visible()) {
            update(
                UpdateRequest::create(UpdateReason::Changed, this)
            );
        }
        else {
            update(
                UpdateRequest::create(UpdateReason::Hidden, this)
            );
        }
    });
}

View::~View()
{
    children_ = 0;
    if (parent_)
        parent_->removeChild(this);
}

Point View::mapToWindow(Point l) const
{
    if (parent_)
        return parent_->mapToWindow(l + pos());
    return l;
}

Point View::mapToLocal(Point p) const
{
    for (
        const View *view = this;
        view->parent_;
        view = view->parent_
    )
       p -= view->pos();

    return p;
}

bool View::contains(Point pp) const
{
    return
        pos()[0] <= pp[0] && pp[0] < pos()[0] + size()[0] &&
        pos()[1] <= pp[1] && pp[1] < pos()[1] + size()[1];
}

void View::mouseEvent(MouseEvent *event)
{
    if (!parent_) {
        if (window_->pointerTarget_) {
            event->pos_ = window_->pointerTarget_->mapToLocal(event->pos());
            window_->pointerTarget_->mouseEvent(event);
            if (event->action() == PointerAction::Released)
                window_->pointerTarget_ = 0;
            return;
        }
    }

    for (auto pair: children_) {
        View *child = pair->value();
        if (child->contains(event->pos())) {
            event->pos_ -= child->pos();
            child->mouseEvent(event);
            if (event->action() == PointerAction::Pressed)
                window_->pointerTarget_ = child;
            return;
        }
    }
}

bool View::isOpaque() const
{
    return Color::isOpaque(color());
}

bool View::isPainted() const
{
    return Color::isValid(color());
}

bool View::isStatic() const
{
    return true;
}

void View::setup()
{}

void View::clear()
{
    image()->clear(Color::premultiplied(color()));
}

void View::paint()
{}

void View::update(const UpdateRequest *request)
{
    if (!window()) return;

    Ref<const UpdateRequest> request_ = request;
    if (!request) request = request_ = UpdateRequest::create(UpdateReason::Changed, this);

    if (!visible() && request->reason() != UpdateReason::Hidden) return;

    if (
        isPainted() && (
            request->reason() == UpdateReason::Changed ||
            request->reason() == UpdateReason::Resized
        )
    ) {
        clear();
        paint();
    }

    window()->addToFrame(request);
}

Window *View::window()
{
    if (!window_) {
        if (parent_)
            window_ = parent_->window();
    }
    return window_;
}

Image *View::image()
{
    if (!image_ || image_->size() != size())
        image_ = Image::create(size());
    return image_;
}

uint64_t View::nextSerial() const
{
    if (children_->count() > 0)
        return children_->keyAt(children_->count() - 1) + 1;
    return 1;
}

void View::insertChild(View *child)
{
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
}

void View::removeChild(View *child)
{
    children_->remove(child->serial_);
}

cairo_surface_t *View::cairoSurface() const
{
    return const_cast<View *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
