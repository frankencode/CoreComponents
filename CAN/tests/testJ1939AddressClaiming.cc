#include <cc/j1939/AddressClaimedRequest>
#include <cc/j1939/AddressClaimedResponse>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::j1939;

    TestCase {
        "AddressClaimedRequest",
        []{
            CanFrame frame = AddressClaimedRequest::produce();
            CC_INSPECT(frame);
            CC_CHECK(AddressClaimedRequest{frame}.isValid());
            CC_INSPECT(AddressClaimedRequest{frame});
        }
    };

    TestCase {
        "AddressClaimedResponse",
        []{
            CanFrame frame = AddressClaimedResponse::produce(NullAddress, 0xA00A80000B202F81u);
            CC_INSPECT(frame);
            CC_CHECK(AddressClaimedResponse{frame}.isValid());
            CC_INSPECT(AddressClaimedResponse{frame});
        }
    };

    return TestSuite{argc, argv}.run();
}
