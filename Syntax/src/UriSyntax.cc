/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/UriSyntax>
#include <cc/InetAddressSyntax>

namespace cc {

struct UriSyntax::State: public SyntaxDefinition::State
{
    InetAddressSyntax inetAddressSyntax_;

    SyntaxRule inetAddress4_ = inetAddressSyntax_.inetAddress4();
    SyntaxRule inetAddress6_ = inetAddressSyntax_.inetAddress6();

    VoidRule alpha_ {
        Choice{
            Within{'a', 'z'},
            Within{'A', 'Z'}
        }
    };

    VoidRule digit_ {
        Within{'0', '9'}
    };

    VoidRule hexDigit_ {
        Choice{
            Within{'0', '9'},
            Choice{
                Within{'A', 'F'},
                Within{'a', 'f'}
            }
        }
    };

    VoidRule delimiter_ {
        OneOf{":/?#[]@"}
    };

    VoidRule punctuator_ {
        OneOf{"!$&'()*+,;="}
    };

    VoidRule unreserved_ {
        Choice{
            Inline{&alpha_},
            Inline{&digit_},
            OneOf{"-._~"}
        }
    };

    VoidRule percentEncoded_ {
        Sequence{
            '%',
            Inline{&hexDigit_},
            Inline{&hexDigit_}
        }
    };

    SyntaxRule pathChar_ {
        Choice{
            &unreserved_,
            &percentEncoded_,
            &punctuator_,
            OneOf{":@"}
        }
    };

    SyntaxRule userInfo_ {
        Repeat{
            Choice{
                &unreserved_,
                &percentEncoded_,
                &punctuator_,
                ':'
            }
        }
    };

    SyntaxRule host_ {
        Choice{
            &inetAddress6_,
            &inetAddress4_,
            Repeat{1,
                Choice{
                    &unreserved_,
                    &percentEncoded_,
                    &punctuator_,
                }
            }
        }
    };

    SyntaxRule port_ {
        Repeat{1, &digit_}
    };

    SyntaxRule scheme_ {
        Sequence{
            &alpha_,
            Repeat{
                Choice{
                    &alpha_,
                    &digit_,
                    OneOf{"+-."}
                }
            }
        }
    };

    SyntaxRule authority_ {
        Sequence{
            Repeat{0, 1,
                Sequence{
                    &userInfo_,
                    '@'
                }
            },
            Choice{
                Sequence{
                    '[',
                    &host_,
                    ']'
                },
                &host_
            },
            Repeat{0, 1,
                Sequence{
                    ':',
                    &port_
                }
            }
        }
    };

    SyntaxRule path_ {
        Sequence{
            Repeat{
                &pathChar_
            },
            Repeat{
                Sequence{
                    '/',
                    Repeat{
                        &pathChar_
                    }
                }
            }
        }
    };

    SyntaxRule query_ {
        Repeat{
            Choice{
                &pathChar_,
                OneOf{"/?"}
            }
        }
    };

    SyntaxRule fragment_ {
        Repeat{
            Choice{
                &pathChar_,
                OneOf{"/?"}
            }
        }
    };

    SyntaxRule uri_ {
        Sequence{
            Repeat{0, 1,
                Sequence{
                    &scheme_,
                    ':',
                    Ahead{"//"}
                }
            },
            Repeat{0, 1,
                Sequence{
                    Repeat{0, 1, "//"},
                    &authority_
                }
            },
            &path_,
            Repeat{0, 1,
                Sequence{
                    '?',
                    &query_
                }
            },
            Repeat{0, 1,
                Sequence{
                    '#',
                    &fragment_
                }
            }
        }
    };

    State():
        SyntaxDefinition::State{&uri_}
    {}
};

UriSyntax::UriSyntax()
{
    initOnce<State>();
}

SyntaxRule UriSyntax::userInfo() const
{
    return me().userInfo_;
}

SyntaxRule UriSyntax::host() const
{
    return me().host_;
}

SyntaxRule UriSyntax::port() const
{
    return me().port_;
}

SyntaxRule UriSyntax::scheme() const
{
    return me().scheme_;
}

SyntaxRule UriSyntax::authority() const
{
    return me().authority_;
}

SyntaxRule UriSyntax::path() const
{
    return me().path_;
}

SyntaxRule UriSyntax::query() const
{
    return me().query_;
}

SyntaxRule UriSyntax::fragment() const
{
    return me().fragment_;
}

SyntaxRule UriSyntax::uri() const
{
    return me().uri_;
}

const UriSyntax::State &UriSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
