#include <cc/ui/Application>
#include <cc/DEBUG>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    View view, sub4;

    View{&view}
    .size(320, 200)
    .add(
        View{}
        .size([=]{ return view.size() / 4; })
        .paper(Color::Red)
    )
    .add(
        View{}
        .size([=]{ return view.size() / 4; })
        .topRight([=]{ return view.topRight(); })
        .paper(Color::Green)
    )
    .add(
        View{}
        .size([=]{ return view.size() / 4; })
        .bottomLeft([=]{ return view.bottomLeft(); })
        .paper(Color::Blue)
    )
    .add(
        View{&sub4}
        .size([=]{ return view.size() / 4; })
        .bottomRight([=]{ return view.bottomRight(); })
        .paper(Color::Black)
        .add(
            View{}
            .size([=]{ return sub4.size() / 2; })
            .centerInParent()
            .paper(Color::White)
        )
    );

    Window window{view};
    window.title([=]{ return str(view.size()); });
    window.show();

    Property<void> debug {[=]{ CC_DEBUG << "pos = " << window.pos() << ", size = " << view.size() << ", isHandheld() = " << view.isHandheld(); }};

    view.onMouseMoved([=](auto event){ CC_DEBUG << event.pos(); return true; });

    return Application{}.run();
}
