/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ImageView>

namespace cc {

ImageView::State::State(const Image &initImage):
    displayImage{initImage}
{
    paper([this]{ return displayImage() ? basePaper() : Color{}; });
    size([this]{ return displayImage() ? displayImage().size() : Size{}; });
    paint([this]{
        Image source = displayImage();
        if (paper().isOpaque()) {
            if (!source.checkOpaque()) {
                source = source.copy();
                source.applyOver(paper());
            }
        }
        image(source);
    });
}

ImageView::ImageView():
    View{onDemand<State>}
{}

ImageView::ImageView(const Image &displayImage):
    View{new ImageView::State{displayImage}}
{}

ImageView &ImageView::associate(Out<ImageView> self)
{
    return View::associate<ImageView>(self);
}

} // namespace cc
