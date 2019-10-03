#include <cc/Format>
#include <cc/can/AbortRequest>

namespace cc {
namespace can {

String AbortRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "AbortRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "  selector: " << selector() << nl
        << "  reason  : " << reason() << nl
        << "}";
}

}} // namespace cc::can
