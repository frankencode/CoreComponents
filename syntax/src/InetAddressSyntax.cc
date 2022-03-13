/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/InetAddressSyntax>

namespace cc {

struct InetAddressSyntax::State: public SyntaxDefinition::State
{
    VoidRule decimalOctet_ {
        Repeat{0, 3, Within{'0', '9'}}
    };

    VoidRule hexWord_ {
        Repeat{1, 4,
            Choice {
                Within{'0', '9'},
                Within{'A', 'F'},
                Within{'a', 'f'}
            }
        }
    };

    SyntaxRule inetAddress4_ {
        Sequence{
            &decimalOctet_,
            '.',
            &decimalOctet_,
            '.',
            &decimalOctet_,
            '.',
            &decimalOctet_
        }
    };

    SyntaxRule inetAddress6_ {
        Sequence {
            Repeat{1, 7,
                Sequence{
                    Choice{
                        &hexWord_,
                        ':'
                    },
                    ':'
                }
            },
            Repeat{0, 1,
                Choice{
                    &inetAddress4_,
                    &hexWord_
                }
            }
        }
    };

    SyntaxRule inetAddress_ {
        Choice{
            &inetAddress4_,
            &inetAddress6_
        }
    };

    State():
        SyntaxDefinition::State{&inetAddress_}
    {}
};

InetAddressSyntax::InetAddressSyntax()
{
    initOnce<State>();
}

SyntaxRule InetAddressSyntax::inetAddress4() const
{
    return me().inetAddress4_;
}

SyntaxRule InetAddressSyntax::inetAddress6() const
{
    return me().inetAddress6_;
}

SyntaxRule InetAddressSyntax::inetAddress() const
{
    return me().inetAddress_;
}

const InetAddressSyntax::State &InetAddressSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
