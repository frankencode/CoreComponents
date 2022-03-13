/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ImageView>

namespace cc::ui {

ImageView::State::State(const Image &initImage):
    displayImage{initImage}
{
    paper([this]{ return displayImage() ? basePaper() : Color{}; });
    size([this]{ return displayImage() ? displayImage().size() : Size{}; });
    paint([this]{
        Image source = displayImage();
        if (!source.checkOpaque()) {
            source = source.copy();
            source.applyOver(paper());
        }
        image(source);
    });
}

ImageView::ImageView():
    View{onDemand<State>}
{}

ImageView::ImageView(Out<ImageView> self):
    View{new ImageView::State}
{
    self = *this;
}

ImageView::ImageView(const Image &displayImage, Out<ImageView> self):
    View{new ImageView::State{displayImage}}
{
    self = *this;
}

} // namespace cc::ui
