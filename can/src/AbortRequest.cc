#include <cc/format>
#include <cc/can/AbortRequest>

namespace cc {
namespace can {

string AbortRequest::Instance::toString() const
{
    if (!isValid()) return string{};

    return format{}
        << "AbortRequest {" << nl
        << "  serverId: " << serverId() << nl
        << "  selector: " << selector() << nl
        << "  reason  : " << reason() << nl
        << "}";
}

}} // namespace cc::can
