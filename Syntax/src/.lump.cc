/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/LongestChoiceNode>
#include <cc/Token>
#include <limits>

namespace cc::syntax {

struct LongestChoiceNode::State: public SyntaxNode::State
{
    explicit State(std::initializer_list<SyntaxNode> choices):
        choices_{Array<SyntaxNode>::allocate(static_cast<long>(choices.size()))}
    {
        choices_ = choices;
    }

    explicit State(const Array<SyntaxNode> &choices):
        choices_{choices}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        SyntaxNode bestEntry;
        long m = -1;
        for (const SyntaxNode &entry: choices_) {
            long h = entry.matchNext(text, offset, production);
            transaction.rollback();
            if (m < h) {
                m = h;
                bestEntry = entry;
            }
        }

        if (bestEntry && !transaction.isEmpty())
            bestEntry.matchNext(text, offset, production);

        return m;
    }

    long matchLength() const override
    {
        long min = std::numeric_limits<long>::max(), max = -1;
        for (const SyntaxNode &entry: choices_) {
            long length = entry.matchLength();
            if (length < min) min = length;
            else if (max < length) max = length;
        }
        return (min != max) ? -1 : max;
    }

    void decycle() override { choices_ = Array<SyntaxNode>{}; }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "LongestChoice{";
        for (const SyntaxNode &choice: choices_)
            parts << choice.explain() << (choice == choices_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> choices_;
};

LongestChoiceNode::LongestChoiceNode(std::initializer_list<SyntaxNode> choices):
    SyntaxNode{new State{choices}}
{}

LongestChoiceNode::LongestChoiceNode(const Array<SyntaxNode> &choices):
    SyntaxNode{new State{choices}}
{}

Array<SyntaxNode> LongestChoiceNode::choices() const
{
    return me().choices_;
}

const LongestChoiceNode::State &LongestChoiceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/LookAheadNode>

namespace cc::syntax {

template class LookAheadNode<0>;
template class LookAheadNode<1>;

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/AnyNode>

namespace cc::syntax {

struct AnyNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return text.has(offset) ? offset + 1 : -1;
    }

    long matchLength() const override
    {
        return 1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Any{}";
    }
};

AnyNode::AnyNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ChoiceNode>
#include <cc/Token>
#include <limits>

namespace cc::syntax {

struct ChoiceNode::State: public SyntaxNode::State
{
    State(std::initializer_list<SyntaxNode> choices):
        choices_{choices}
    {}

    State(const Array<SyntaxNode> &choices):
        choices_{choices}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long h = -1;
        for (const SyntaxNode &entry: choices_) {
            h = entry.matchNext(text, offset, production);
            if (h == -1) transaction.rollback();
            else break;
        }

        return h;
    }

    long matchLength() const override
    {
        long min = std::numeric_limits<long>::max(), max = -1;
        for (const SyntaxNode &entry: choices_) {
            long length = entry.matchLength();
            if (length < min) min = length;
            else if (max < length) max = length;
        }
        return (min != max) ? -1 : max;
    }

    void decycle() override { choices_ = Array<SyntaxNode>{}; }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "Choice{";
        for (const SyntaxNode &choice: choices_)
            parts << choice.explain() << (choice == choices_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> choices_;
};

ChoiceNode::ChoiceNode(std::initializer_list<SyntaxNode> choices):
    SyntaxNode{new State{choices}}
{}

ChoiceNode::ChoiceNode(const Array<SyntaxNode> &choices):
    SyntaxNode{new State{choices}}
{}

Array<SyntaxNode> ChoiceNode::choices() const
{
    return me().choices_;
}

const ChoiceNode::State &ChoiceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ReplayNode>
#include <cc/Token>

namespace cc::syntax {

struct ReplayNode::State: public SyntaxNode::State
{
    State(const String &name):
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        String scribble = production.capturedValue(name_);
        long j = 0;
        long m = scribble.count();
        for (long i = offset, n = text.count(); i < n && j < m; ++i) {
            if (text.at(i) != scribble.at(j)) break;
            ++j;
        }

        return (j < m) ? -1 : (offset + m);
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Replay{\"" << name_ << "\"}";
    }

    String name_;
};

ReplayNode::ReplayNode(const String &name):
    SyntaxNode{new State{name}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/EoiNode>

namespace cc::syntax {

struct EoiNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        bool eoi = (!text.has(offset)) && ((offset == 0) || (text.has(offset - 1)));
        return eoi ? offset : -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Eoi{}";
    }
};

EoiNode::EoiNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/RepeatNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct RepeatNode::State: public SyntaxNode::State
{
    State(long minRepeat, long maxRepeat, const SyntaxNode &entry):
        minRepeat_{minRepeat},
        maxRepeat_{maxRepeat},
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long repeatCount = 0;
        for (; repeatCount < maxRepeat_; ++repeatCount) {
            long h = entry_.matchNext(text, offset, production);
            if (h == -1) break;
            if (h == offset) {
                transaction.rollback();
                return -1;
            }
            offset = h;
        }

        if (repeatCount < minRepeat_ || maxRepeat_ < repeatCount)
            offset = -1;

        if (offset == -1) transaction.rollback();

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Repeat{" << str(minRepeat_) << "," << str(maxRepeat_) << "," << entry_.explain() << "}";
    }

    long minRepeat_;
    long maxRepeat_;
    SyntaxNode entry_;
};

RepeatNode::RepeatNode(long minRepeat, long maxRepeat, const SyntaxNode &entry):
    SyntaxNode{new State{minRepeat, maxRepeat, entry}}
{}

RepeatNode::RepeatNode(long minRepeat, const SyntaxNode &entry):
    SyntaxNode{new State{minRepeat, std::numeric_limits<long>::max(), entry}}
{}

RepeatNode::RepeatNode(const SyntaxNode &entry):
    SyntaxNode{new State{0, std::numeric_limits<long>::max(), entry}}
{}

long RepeatNode::minRepeat() const
{
    return me().minRepeat_;
}

long RepeatNode::maxRepeat() const
{
    return me().maxRepeat_;
}

SyntaxNode RepeatNode::entry() const
{
    return me().entry_;
}

const RepeatNode::State &RepeatNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/RangeMinMaxNode>

namespace cc::syntax {

template class RangeMinMaxNode<0>;
template class RangeMinMaxNode<1>;

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/RefNode>
#include <cc/SyntaxRule>
#include <cc/str>
#include <cassert>

namespace cc::syntax {

struct RefNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule):
        rule_{rule}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return rule_->matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return rule_->matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "Ref{" << str(static_cast<const void *>(rule_)) << "}";
    }

    const SyntaxRule *rule_ { nullptr };
};

RefNode::RefNode(const SyntaxRule *rule):
    SyntaxNode{new State{rule}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/LengthNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct LengthNode::State: public SyntaxNode::State
{
    State(long minLength, long maxLength, const SyntaxNode &entry):
        minLength_{minLength},
        maxLength_{maxLength},
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long h = entry_.matchNext(text, offset, production);
        if (h != -1) {
            long d = h - offset;
            if ((d < minLength_) || (maxLength_ < d))
                h = -1;
        }

        if (h == -1) transaction.rollback();

        return h;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Length{" << str(minLength_) << "," << str(maxLength_) << "," << entry_.explain() << "}";
    }

    long minLength_;
    long maxLength_;
    SyntaxNode entry_;
};

LengthNode::LengthNode(long minLength, long maxLength, const SyntaxNode &entry):
    SyntaxNode{new State{minLength, maxLength, entry}}
{}

/** \param minLength Required minimum length of the sub-expression match
  * \param entry Sub-expression to match
  */
LengthNode::LengthNode(long minLength, const SyntaxNode &entry):
    SyntaxNode{new State{minLength, std::numeric_limits<long>::max(), entry}}
{}

long LengthNode::minLength() const
{
    return me().minLength_;
}

long LengthNode::maxLength() const
{
    return me().maxLength_;
}

SyntaxNode LengthNode::entry() const
{
    return me().entry_;
}

const LengthNode::State &LengthNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ExpectNode>
#include <cc/syntax/CharNode>
#include <cc/syntax/LiteralNode>
#include <cc/Format>

namespace cc::syntax {

struct ExpectNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &error):
        entry_{entry},
        error_{error}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long i = entry_.matchNext(text, offset, production);

        if (i == -1) {
            String message = error_;
            if (message == "") {
                String hint = entry_.hint();
                if (hint != "") message = Format{"Expected \"%%\""} << hint;
            }
            throw TextError{text, offset, message};
        }

        return i;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "Expect{" << entry_.explain() << ",\"" << error_ << "\"" << "}";
    }

    SyntaxNode entry_;
    String error_;
};

ExpectNode::ExpectNode(const SyntaxNode &entry, const String &error):
    SyntaxNode{new State{entry, error}}
{}

ExpectNode::ExpectNode(char ch, const String &error):
    ExpectNode{CharNode{ch}, error}
{}

ExpectNode::ExpectNode(const String &s, const String &error):
    ExpectNode{LiteralNode{s}, error}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/SequenceNode>
#include <cc/Token>

namespace cc::syntax {

struct SequenceNode::State: public SyntaxNode::State
{
    State(std::initializer_list<SyntaxNode> nodes):
        nodes_{nodes}
    {}

    State(const Array<SyntaxNode> &nodes):
        nodes_{nodes}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        for (const SyntaxNode &entry: nodes_) {
            offset = entry.matchNext(text, offset, production);
            if (offset == -1) {
                transaction.rollback();
                break;
            }
        }

        return offset;
    }

    long matchLength() const override
    {
        long totalLength = 0;
        for (const SyntaxNode &entry: nodes_) {
            long length = entry.matchLength();
            if (length >= 0) totalLength += length;
            else {
                totalLength = -1;
                break;
            }
        }
        return totalLength;
    }

    void decycle() override { nodes_ = Array<SyntaxNode>{}; }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "Sequence{";
        for (const SyntaxNode &entry: nodes_)
            parts << entry.explain() << (entry == nodes_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> nodes_;
};

SequenceNode::SequenceNode(std::initializer_list<SyntaxNode> nodes):
    SyntaxNode{new State{nodes}}
{}

SequenceNode::SequenceNode(const Array<SyntaxNode> &nodes):
    SyntaxNode{new State{nodes}}
{}

Array<SyntaxNode> SequenceNode::nodes() const
{
    return me().nodes_;
}

const SequenceNode::State &SequenceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/MatchNode>

namespace cc::syntax {

struct MatchNode::State: public SyntaxNode::State
{
    State(Fun<bool(char)> &&match):
        match_{match}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        if (text.has(offset)) {
            char ch = text.at(offset++);
            if (!match_(ch)) offset = -1;
        }
        else
            offset = -1;

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Match{...}";
    }

    const Fun<bool(char)> match_;
};

MatchNode::MatchNode(Fun<bool(char)> &&match):
    SyntaxNode{new State{move(match)}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SyntaxRule>
#include <cc/syntax/VoidRule>
#include <cc/Token>

namespace cc {

struct SyntaxRule::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    State(const String &name, const SyntaxNode &entry):
        entry_{entry},
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        Token token{&production};
        token.setRule(Object::alias<SyntaxRule>(this));

        Range range{offset};
        range[1] = entry_.matchNext(text, range[0], token);

        if (range[1] != -1) {
            token.setRange(range);
            production.children().emplaceBack(move(token));
        }

        return range[1];
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "SyntaxRule{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
    Object metaData_;
    String name_;
};

SyntaxRule::SyntaxRule(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxRule::SyntaxRule(const String &name, const SyntaxNode &entry):
    SyntaxNode{new State{name, entry}}
{}

SyntaxRule::SyntaxRule(State *newState):
    SyntaxNode{newState}
{}

String SyntaxRule::name() const
{
    return me().name_;
}

void SyntaxRule::setName(const String &newName)
{
    me().name_ = newName;
}

syntax::SyntaxNode SyntaxRule::entry() const
{
    return me().entry_;
}

Object SyntaxRule::metaData() const
{
    return me().metaData_;
}

void SyntaxRule::setMetaData(const Object &newValue)
{
    me().metaData_ = newValue;
}

SyntaxRule::State &SyntaxRule::me()
{
    return Object::me.as<State>();
}

const SyntaxRule::State &SyntaxRule::me() const
{
    return Object::me.as<State>();
}

} // namespace cc

namespace cc::syntax {

struct VoidRule::State: public SyntaxRule::State
{
    State(const SyntaxNode &entry):
        SyntaxRule::State{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        Token::Children::Transaction transaction { production.children() };
        long newOffset = entry_.matchNext(text, offset, production);
        transaction.rollback();
        return newOffset;
    }

    List<String> explain() const override
    {
        return List<String>{} << "VoidRule{" << entry_.explain() << "}";
    }
};

VoidRule::VoidRule(const SyntaxNode &entry):
    SyntaxRule{new State{entry}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IntegerSyntax>
#include <cc/InOut>

namespace cc {

struct IntegerSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule plusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule binary {
        Sequence {
            Literal{"0b"},
            Repeat{1, 256, OneOf{'0', '1'}}
        }
    };

    SyntaxRule octal {
        Sequence {
            Char{'0'},
            Repeat{1, 24, Within{'0', '9'}}
        }
    };

    SyntaxRule hexadecimal {
        Sequence {
            Literal{"0x"},
            Repeat{1, 20,
                Choice{
                    Within{'0', '9'},
                    Within{'a', 'f'},
                    Within{'A', 'F'}
                }
            }
        }
    };

    SyntaxRule decimal {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule integer {
        Sequence{
            Repeat{0, 1, &plusMinus},
            Choice{
                &binary,
                &octal,
                &hexadecimal,
                &decimal
            },
            Not{
                OneOf{'.', 'e', 'E'}
            }
        }
    };

    State():
        SyntaxDefinition::State{&integer}
    {}

    void read(Out<uint64_t> value, Out<int> sign, const String &text, const Token &token) const
    {
        sign = 1;
        value = 0;

        Token child = token.children().first();

        if (child.rule() == plusMinus)
        {
            if (text.at(child) == '-') sign = -1;
            child = *(++token.children().begin());
        }

        if (child.rule() == decimal)
        {
            for (long i = child.range()[0]; i < child.range()[1]; ++i)
            {
                value *= 10;
                value += text.at(i) - '0';
            }
        }
        else if (child.rule() == hexadecimal)
        {
            for (long i = child.range()[0] + 2; i < child.range()[1]; ++i)
            {
                char x = text.at(i);
                if (('0' <= x) && (x <= '9')) x -= '0';
                else if (('a' <= x) && (x <= 'z')) x -= 'a' - 10;
                else if (('A' <= x) && (x <= 'Z')) x -= 'A' - 10;
                value *= 16;
                value += x;
            }
        }
        else if (child.rule() == binary)
        {
            for (long i = child.range()[0] + 2; i < child.range()[1]; ++i)
            {
                value *= 2;
                value += text.at(i) - '0';
            }
        }
        else if (child.rule() == octal)
        {
            for (long i = child.range()[0] + 1; i < child.range()[1]; ++i)
            {
                value *= 8;
                value += text.at(i) - '0';
            }
        }
    }

    Token read(Out<uint64_t> value, Out<int> sign, const String &text, long offset) const
    {
        String section = (offset == 0) ? text : String{text}.select(offset, text.count());
        Token token = match(section);
        if (token) read(value, sign, text, token);
        return token;
    }
};

IntegerSyntax::IntegerSyntax()
{
    initOnce<State>();
}

void IntegerSyntax::read(Out<uint64_t> value, Out<int> sign, const String &text, const Token &token) const
{
    me().read(value, sign, text, token);
}

Token IntegerSyntax::read(Out<uint64_t> value, Out<int> sign, const String &text, long offset) const
{
    return me().read(value, sign, text, offset);
}

const IntegerSyntax::State &IntegerSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/FindNode>
#include <cc/Token>

namespace cc::syntax {

struct FindNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        for (; offset <= text.count(); ++offset) {
            long h = entry_.matchNext(text, offset, production);
            if (h != -1) {
                offset = h;
                break;
            }
        }

        if (offset == -1) transaction.rollback();

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "Find{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
};

FindNode::FindNode(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxNode FindNode::entry() const
{
    return me().entry_;
}

const FindNode::State &FindNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/PassNode>

namespace cc::syntax {

struct PassNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Pass{}";
    }

};

PassNode::PassNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Token>
#include <cc/Map>
#include <cc/TokenScreen>

namespace cc {

struct Token::MatchState
{
    Map<String, String> captures_;
};

String Token::State::capturedValue(const String &name) const
{
    const State *root = getRoot();
    if (!root) return String{};
    return root->matchState_->captures_.value(name);
}

void Token::State::setCapturedValue(const String &name, const String &value)
{
    const State *root = getRoot();
    if (!root->matchState_) root->matchState_ = new MatchState;
    root->matchState_->captures_.establish(name, value);
}

void Token::State::deleteMatchState()
{
    delete matchState_;
}

bool Token::project(const TokenScreen &screen) const
{
    TokenScreen target = screen;
    return projectCascade(target);
}

bool Token::projectCascade(TokenScreen &screen) const
{
    long i = range()[0];

    for (const Token &child: children())
    {
        if (i < child.range()[0]) {
            if (!screen.project(*this, i, child.range()[0]))
                return false;
            i = child.range()[0];
        }
        if (!child.projectCascade(screen))
            return false;

        i = child.range()[1];
    }

    if (i < range()[1]) {
        if (!screen.project(*this, i, range()[1]))
            return false;
    }

    return true;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/InlineNode>
#include <cc/SyntaxRule>
#include <cc/str>
#include <cassert>

namespace cc::syntax {

struct InlineNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule):
        rule_{rule}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return rule_->entry().matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return rule_->entry().matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "Inline{" << str(static_cast<const void *>(rule_)) << "}";
    }

    const SyntaxRule *rule_ { nullptr };
};

InlineNode::InlineNode(const SyntaxRule *rule):
    SyntaxNode{new State{rule}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/DebugNode>
#include <cc/stdio>

namespace cc::syntax {

struct DebugNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &label):
        entry_{entry},
        label_{label}
    {
        if (label_ == "") label_ = "DEBUG";
    }

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long newOffset = entry_.matchNext(text, offset, production);
        String chunk;
        if (0 <= offset && offset < newOffset && newOffset <= text.count()) chunk = text.copy(offset, newOffset);
        ferr() << label_ << ":" << offset << ".." << newOffset << ": \"" << chunk << "\""<< nl;
        return newOffset;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "Debug{" << entry_.explain() << ",\"" << label_ << "\"" << "}";
    }

    SyntaxNode entry_;
    String label_;
};

DebugNode::DebugNode(const SyntaxNode &entry, const String &label):
    SyntaxNode{new State{entry, label}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/FindLastNode>
#include <cc/Token>

namespace cc::syntax {

struct FindLastNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long offsetLast = -1;
        for (; offset <= text.count(); ++offset) {
            long h = entry_.matchNext(text, offset, production);
            if (h != -1) offsetLast = h;
        }

        if (offsetLast == -1) transaction.rollback();

        return offsetLast;
    }

    long matchLength() const override
    {
        return -1;
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "FindLast{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
};

FindLastNode::FindLastNode(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxNode FindLastNode::entry() const
{
    return me().entry_;
}

const FindLastNode::State &FindLastNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/StringNode>

namespace cc::syntax {

template class StringNode<0>;
template class StringNode<1>;

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ContextNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct ContextNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule, const SyntaxNode &inside, const SyntaxNode &outside):
        rule_{rule},
        inside_{inside},
        outside_{outside}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return production.withinContext(*rule_) ?
            inside_.matchNext(text, offset, production) :
            outside_.matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Context{" << str(static_cast<const void *>(rule_)) << "," << inside_.explain() << "," << outside_.explain() << "}";
    }

    const SyntaxRule *rule_ { nullptr };
    SyntaxNode inside_;
    SyntaxNode outside_;
};

ContextNode::ContextNode(const SyntaxRule *context, const SyntaxNode &inside, const SyntaxNode &outside):
    SyntaxNode{new State{context, inside, outside}}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FloatSyntax>
#include <limits>
#include <cmath>

namespace cc {

struct FloatSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule plusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule integralPart {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule fractionalPart {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule exponentPlusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule exponent {
        Repeat{1, 3,
            Within{'0', '9'}
        }
    };

    SyntaxRule notANumber {
        Casefree{"nan"}
    };

    SyntaxRule infinite {
        Sequence{
            Repeat{0, 1, Char{'-'}},
            Casefree{"inf"},
            Repeat{0, 1, Casefree{"inite"}}
        }
    };

    SyntaxRule floatingPoint {
        Choice{
            Sequence{
                Repeat{0, 1, &plusMinus},
                Choice{
                    Sequence{
                        &integralPart,
                        Repeat{0, 1,
                            Sequence{
                                Char{'.'},
                                Repeat{0, 1, &fractionalPart}
                            }
                        }
                    },
                    Sequence{
                        Char{'.'},
                        &fractionalPart
                    }
                },
                Repeat{0, 1,
                    Sequence{
                        OneOf{'e', 'E'},
                        Repeat{0, 1, &exponentPlusMinus},
                        &exponent
                    }
                },
                Not{
                    OneOf{'.', 'e', 'E'}
                }
            },
            Sequence{
                Choice{
                    &notANumber,
                    &infinite
                },
                Not{
                    Choice{
                        Within{'a', 'z'},
                        Within{'A', 'Z'}
                    }
                }
            }
        }
    };

    State():
        SyntaxDefinition::State{&floatingPoint}
    {}

    void read(Out<double> value, const String &text, const Token &token) const
    {
        auto it = token.children().begin();
        Token child = *it;

        if (child.rule() == notANumber)
        {
            value = std::numeric_limits<double>::quiet_NaN();
        }
        else if (child.rule() == infinite)
        {
            double one, zero;
            one = 1.; zero = 0.;

            if (text.at(child) == '-')
                value = -one / zero;
            else
                value = one / zero;
        }
        else {
            int sign = 1;
            double mantissa = 0.;
            int epSign = 1;
            int ep = 0;

            for (; it; ++it)
            {
                Token child = *it;

                if (child.rule() == plusMinus)
                {
                    if (text.at(child) == '-')
                        sign = -1;
                }
                else if (child.rule() == integralPart)
                {
                    for (long j = child.range()[0]; j < child.range()[1]; ++j) {
                        mantissa *= 10;
                        mantissa += text.at(j) - '0';
                    }
                }
                else if (child.rule() == fractionalPart)
                {
                    double h = 0.1;
                    for (long j = child.range()[0]; j < child.range()[1]; ++j) {
                        mantissa += h * (text.at(j) - '0');
                        h /= 10;
                    }
                }
                else if (child.rule() == exponentPlusMinus)
                {
                    if (text.at(child) == '-')
                        epSign = -1;
                }
                else if (child.rule() == exponent)
                {
                    for (long j = child.range()[0]; j < child.range()[1]; ++j)
                    {
                        ep *= 10;
                        ep += text.at(j) - '0';
                    }
                }
            }

            value = sign * mantissa;
            if (ep != 0)
                value *= std::pow(10., double(epSign * ep));
        }
    }

    Token read(Out<double> value, const String &text, double offset) const
    {
        String section = offset == 0 ? text : String{text}.select(offset, text.count());
        Token token = match(section);
        if (token) read(value, text, token);
        return token;
    }
};

FloatSyntax::FloatSyntax()
{
    initOnce<State>();
}

void FloatSyntax::read(Out<double> value, const String &text, const Token &token) const
{
    me().read(value, text, token);
}

Token FloatSyntax::read(Out<double> value, const String &text, double offset) const
{
    return me().read(value, text, offset);
}

const FloatSyntax::State &FloatSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/SyntaxNode>
#include <cc/Token>

namespace cc::syntax {

void SyntaxNode::State::decycle()
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/CharCompareNode>

namespace cc::syntax {

template class CharCompareNode<Equal>;
template class CharCompareNode<Differ>;

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/KeywordNode>

namespace cc::syntax {

struct KeywordNode::State: public SyntaxNode::State
{
    State(const Set<String> &keywords):
        keywords_{keywords}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        if (text.has(offset)) {
            Prefix prefix{text, offset};
            Locator pos;
            if (keywords_.find(prefix, &pos))
                offset += keywords_.at(pos).count();
            else
                offset = -1;
        }
        else
            offset = -1;

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "Keyword{";
        for (const String &keyword: keywords_) {
            parts << "\"" << keyword << "\",";
        }
        parts << "}";
        return parts;
    }

    Set<String> keywords_;
};

KeywordNode::KeywordNode(std::initializer_list<String> keywords):
    SyntaxNode{new State{keywords}}
{}

Set<String> KeywordNode::keywords() const
{
    return me().keywords_;
}

const KeywordNode::State &KeywordNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/FailNode>

namespace cc::syntax {

struct FailNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Fail{}";
    }
};

FailNode::FailNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/RangeExplicitNode>

namespace cc::syntax {

template class RangeExplicitNode<0>;
template class RangeExplicitNode<1>;

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/BoiNode>

namespace cc::syntax {

struct BoiNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return (offset == 0) ? offset : -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Boi{}";
    }
};

BoiNode::BoiNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/CaptureNode>
#include <cc/Token>

namespace cc::syntax {

struct CaptureNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &name):
        entry_{entry},
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long h = entry_.matchNext(text, offset, production);
        if (h >= 0) production.setCapturedValue(name_, text.copy(offset, h));
        return h;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Capture{" << entry_.explain() << ",\"" << name_ << "\"}";
    }

    SyntaxNode entry_;
    String name_;
};

CaptureNode::CaptureNode(const SyntaxNode &entry, const String &name):
    SyntaxNode{new State{entry, name}}
{}

} // namespace cc::syntax
