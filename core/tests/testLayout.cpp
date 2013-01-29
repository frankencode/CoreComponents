#include <ftl/LayoutTree.hpp>
#include <ftl/property.hpp>

namespace testLayout
{

using namespace ftl;

class Line {
public:
	typedef int Extend;
	property<Extend> extend;
};

} // namespace testLayout

int main(int argc, char **argv)
{
	using namespace testLayout;

	LayoutTree< LayoutNode<Line> > linesLayout_;

	return 0;
}
