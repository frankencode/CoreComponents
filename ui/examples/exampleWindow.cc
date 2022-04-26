#include <cc/Application>
#include <cc/DEBUG>

int main()
{
    using namespace cc;

    View view, sub4;

    Window{
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
        )
        .attach({[=]{
            Window window = view.window();
            if (window) {
                CC_DEBUG << "pos = " << window.pos() << ", size = " << window.size() << ", isHandheld() = " << view.isHandheld();
            }
            else {
                CC_DEBUG << "no window";
            }
        }})
        .onMouseMoved([=](auto event){
            CC_DEBUG << event.pos();
            return true;
        })
    }
    .title([=]{ return str(view.size()); })
    .show();

    return Application{}.run();
}
