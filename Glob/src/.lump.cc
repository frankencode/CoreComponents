/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PatternSyntax>
#include <cc/Array>
#include <cc/Queue>
#include <cc/input>

namespace cc {

struct PatternSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule any { '#' };

    SyntaxRule gap { '*' };

    SyntaxRule boi { Sequence { Boi{}, '^' } };

    SyntaxRule eoi { Sequence { '^', Eoi{} } };

    SyntaxRule character {
        Choice {
            NoneOf{'#', '*', '\\', '[', ']', '(', ')', '{', '}', '|', '^'},
            Sequence{
                '\\',
                Expect{
                    Choice{
                        OneOf{
                            '#', '*', '\\', '[', ']', '(', ')', '{', '}', '|', '^', ':',
                            'n', 's', 't', 'r', 'f'
                        },
                        Sequence{
                            'x',
                            Repeat{2, 2,
                                Choice{
                                    Within{'0', '9'},
                                    Within{'a', 'f'},
                                    Within{'A', 'F'}
                                }
                            }
                        }
                    },
                    "Invalid escape sequence"
                }
            }
        }
    };

    SyntaxRule string {  Repeat{2, &character} };

    SyntaxRule rangeMinMax {
        Sequence {
            '[',
            Choice{
                Sequence{
                    Repeat{0, 1, '^'},
                    Choice{
                        Sequence{ "..", &character },
                        Sequence{ &character, "..", &character },
                        Sequence{ &character, ".." }
                    }
                },
                ".."
            },
            Expect{']'}
        }
    };

    SyntaxRule rangeExplicit {
        Sequence{
            '[',
            Repeat{0, 1, '^'},
            Repeat{1, &character},
            Expect{']'}
        }
    };

    SyntaxRule range {
        Sequence{
            Ahead{'['},
            Expect{
                Choice{
                    &rangeMinMax,
                    &rangeExplicit
                },
                "Expected range definition"
            }
        }
    };

    SyntaxRule number {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule minRepeat { Inline{&number} };
    SyntaxRule maxRepeat { Inline{&number} };

    SyntaxRule repeat {
        Sequence {
            '{',
            Repeat{0, 1,
                Sequence{
                    Repeat{0, 1, &minRepeat},
                    "..",
                    Repeat{0, 1, &maxRepeat},
                    Expect{':'}
                }
            },
            Expect{
                &choice,
                "Expected repeat expression"
            },
            Expect{'}'}
        }
    };

    SyntaxRule sequence {
        Repeat{
            Choice{
                &repeat,
                &string,
                &character,
                &any,
                &gap,
                Inline{&range},
                &boi,
                &eoi,
                &group,
                &behind,
                &ahead
            }
        }
    };

    SyntaxRule group {
        Sequence{
            '(',
            Not{OneOf{'>', '<', '^'}},
            &choice,
            Expect{')'}
        }
    };

    SyntaxRule ahead {
        Sequence{
            '(',
            Repeat{0, 1, '^'},
            '>',
            Expect{
                &choice,
                "Expected ahead expression"
            },
            Expect{')'}
        }
    };

    SyntaxRule behind {
        Sequence{
            '(',
            Repeat{0, 1, '^'},
            '<',
            Expect{
                &choice,
                "Expected begin expression"
            },
            Expect{')'}
        }
    };

    SyntaxRule choice {
        Sequence{
            &sequence,
            Repeat{
                Sequence{
                    '|',
                    &sequence
                }
            }
        }
    };

    SyntaxRule pattern {
        Sequence{
            &choice,
            Eoi{}
        }
    };

    State():
        SyntaxDefinition::State{&pattern}
    {}

    SyntaxNode compile(const String &text) const
    {
        if (text.count() == 0) return Pass{};

        Token token = match(text);
        if (!token) throw TextError{text, text.count(), "Illegal expression syntax"};

        return compileChoice(text, token.children().first());
    }

    SyntaxNode compileSequence(const String &text, const Token &token) const
    {
        return compileSequence(text, token, token.children().begin());
    }

    SyntaxNode compileSequence(const String &text, const Token &token, Token::Children::iterator head) const
    {
        Queue<SyntaxNode> nodes;

        for (auto it = head; it; ++it) {
            const Token &child = *it;
            if (child.rule() == string) nodes.pushBack(Literal{readString(text, child)});
            else if (child.rule() == character) nodes.pushBack(Char{readChar(text, child)});
            else if (child.rule() == any) nodes.pushBack(Any{});
            else if (child.rule() == gap) {
                auto succ = it + 1;
                if (succ) {
                    nodes.pushBack(FindLast{compileSequence(text, token, succ)});
                    break;
                }
                else nodes.pushBack(Repeat{Any{}});
            }
            else if (child.rule() == rangeMinMax) nodes.pushBack(compileRangeMinMax(text, child));
            else if (child.rule() == rangeExplicit) nodes.pushBack(compileRangeExplicit(text, child));
            else if (child.rule() == repeat) nodes.pushBack(compileRepeat(text, child));
            else if (child.rule() == boi) nodes.pushBack(Boi{});
            else if (child.rule() == eoi) nodes.pushBack(Eoi{});
            else if (child.rule() == group) nodes.pushBack(compileGroup(text, child.children().first()));
            else if (child.rule() == ahead) nodes.pushBack(compileAhead(text, child));
            else if (child.rule() == behind) nodes.pushBack(compileBehind(text, child));
        }

        if (nodes.count() == 1) return nodes.first();

        return Sequence{nodes.toArray<SyntaxNode>()};
    }

    SyntaxRule compileGroup(const String &text, const Token &token) const
    {
        return SyntaxRule{compileChoice(text, token)};
    }

    SyntaxNode compileChoice(const String &text, const Token &token) const
    {
        if (token.children().count() == 1)
            return compileSequence(text, token.children().first());

        Array<SyntaxNode> choices = Array<SyntaxNode>::allocate(token.children().count());
        long i = 0;
        for (const Token &child: token.children()) {
            choices[i++] = compileSequence(text, child);
        }

        return LongestChoice{choices};
    }

    SyntaxNode compileAhead(const String &text, const Token &token) const
    {
        return (text.at(token.i0() + 1) != '^') ?
            Ahead{compileChoice(text, token.children().first())}.as<SyntaxNode>() :
            Not{compileChoice(text, token.children().first())}.as<SyntaxNode>();
    }

    SyntaxNode compileBehind(const String &text, const Token &token) const
    {
        return (text.at(token.i0() + 1) != '^') ?
            Behind{compileChoice(text, token.children().first())}.as<SyntaxNode>() :
            NotBehind{compileChoice(text, token.children().first())}.as<SyntaxNode>();
    }

    char readChar(const String &text, const Token &token) const
    {
        if (token.i1() - token.i0() > 1) {
            String h = text.copy(token);
            h.expand();
            return h.at(0);
        }
        return text.at(token.i0());
    }

    String readString(const String &text, const Token &token) const
    {
        String s = String::allocate(token.children().count());
        long i = 0;
        for (const Token &child: token.children())
            s[i++] = readChar(text, child);
        return s;
    }

    SyntaxNode compileRangeMinMax(const String &text, const Token &token) const
    {
        bool invert = (text.at(token.i0() + 1) == '^');
        long n = token.children().count();
        if (n == 2) {
            char a = readChar(text, token.children().first());
            char b = readChar(text, token.children().last());
            return invert ? Outside{a, b}.as<SyntaxNode>() : Within{a, b}.as<SyntaxNode>();
        }
        else if (n == 1) {
            Token child = token.children().first();
            char ch = readChar(text, child);
            if (invert) {
                if (child.i0() - token.i0() <= 2)
                    return Match{[ch](char x) { return x <  ch; }};
                else
                    return Match{[ch](char x) { return x >  ch; }};
            }
            else {
                if (child.i0() - token.i0() <= 2)
                    return Match{[ch](char x) { return x >= ch; }};
                else
                    return Match{[ch](char x) { return x <= ch; }};
            }
        }
        return Any{};
    }

    SyntaxNode compileRangeExplicit(const String &text, const Token &token) const
    {
        bool invert = (text.at(token.i0() + 1) == '^');

        long i = 0, n = token.children().count();
        String s = String::allocate(n);
        for (const Token &child: token.children())
            s[i++] = readChar(text, child);

        return invert ? NoneOf{s}.as<SyntaxNode>() : OneOf{s}.as<SyntaxNode>();
    }

    SyntaxNode compileRepeat(const String &text, const Token &token) const
    {
        long minRepeatCount = 0;
        long maxRepeatCount = std::numeric_limits<long>::max();

        for (const Token &child: token.children()) {
            if (child.rule() == minRepeat) minRepeatCount = readNumber<long>(text.copy(child));
            else if (child.rule() == maxRepeat) maxRepeatCount = readNumber<long>(text.copy(child));
            else break;
        }

        return Repeat{
            minRepeatCount, maxRepeatCount,
            compileChoice(text, token.children().last())
        };
    }
};

PatternSyntax::PatternSyntax()
{
    initOnce<State>();
}

SyntaxRule PatternSyntax::compile(const String &text) const
{
    return SyntaxRule{me().compile(text)};
}

const PatternSyntax::State &PatternSyntax::me() const
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

#include <cc/Pattern>
#include <cc/PatternSyntax>

namespace cc {

struct Pattern::State final: public Object::State
{
    State(const String &text):
        definition_{&rule_},
        rule_{PatternSyntax{}.compile(text)},
        text_{text}
    {}

    SyntaxDefinition definition_;
    SyntaxRule rule_;
    String text_;
};

Pattern::Pattern(const String &text):
    Object{new State{text}}
{}

bool Pattern::match(const String &text, Out< List<Range> > captures) const
{
    Token token = me().definition_.match(text);
    if (captures.requested()) {
        captures = List<Range>{};
        if (token) {
            for (const Token &child: token.children()) {
                captures().append(child.range());
            }
        }
    }
    return token;
}

Range Pattern::findIn(const String &text, Out<long> offset) const
{
    Token token = me().definition_.findIn(text, offset());
    Range range;
    if (token) {
        offset = token.i0();
        range = token.range();
    }
    return range;
}

List<String> Pattern::breakUp(const String &text) const
{
    return me().definition_.breakUp(text);
}

long Pattern::matchLength() const
{
    return me().definition_.entry().matchLength();
}

List<String> Pattern::explain() const
{
    return me().rule_.explain();
}

String Pattern::text() const
{
    return me().text_;
}

const Pattern::State &Pattern::me() const
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

#include <cc/Uri>
#include <cc/UriSyntax>
#include <cc/input>

namespace cc {

void Uri::State::readUri(const UriSyntax &syntax, const String &text, const Token &token)
{
    for (const Token &child: token.children())
    {
        if (child.rule() == syntax.scheme()) {
            scheme_ = Uri::decode(text.copy(child));
            scheme_.downcase();
        }
        else if (child.rule() == syntax.authority()) {
            readAuthority(syntax, text, child);
        }
        else if (child.rule() == syntax.path()) {
            path_ = Uri::decode(text.copy(child));
        }
        else if (child.rule() == syntax.query()) {
            query_ = Uri::decode(text.copy(child));
        }
        else if (child.rule() == syntax.fragment()) {
            fragment_ = Uri::decode(text.copy(child));
        }
    }
}

void Uri::State::readAuthority(const UriSyntax &syntax, const String &text, const Token &token)
{
    for (const Token &child: token.children())
    {
        if (child.rule() == syntax.userInfo()) {
            userInfo_ = Uri::decode(text.copy(child));
        }
        else if (child.rule() == syntax.host()) {
            hostIsNumeric_ = (child.children().count() > 0);
            String span = text.copy(child);
            if (!hostIsNumeric_) span.downcase();
            host_ = Uri::decode(span);
        }
        else if (child.rule() == syntax.port()) {
            port_ = readNumber<int>(text.copy(child));
        }
    }
}

String Uri::State::requestHost() const
{
    return host_ + ":" + str(port_);
}

String Uri::State::requestPath() const
{
    if (query_ == "" && fragment_ == "") return path_;
    List<String> parts;
    parts << path_;
    if (query_ != "") parts << "?" << query_;
    if (fragment_ != "") parts << "#" << fragment_;
    return parts;
}

String Uri::State::toString() const
{
    List<String> text;

    if (scheme_ != "") {
        text << scheme_;
        text << ":";
    }
    if (host_ != "") {
        text << "//";
        if (userInfo_ != "") {
            text << userInfo_;
        }
        if (host_ != "") {
            text << "@";
            text << host_;
        }
        if (port_ != -1) {
            text << ":" << str(port_);
        }
    }
    text << path_;
    if (query_ != "") {
        text << "?";
        text << query_;
    }
    if (fragment_ != "") {
        text << "#";
        text << fragment_;
    }

    return text;
}

String Uri::State::encoded() const
{
    List<String> text;

    if (scheme_ != "") {
        text << scheme_;
        text << ":";
    }
    if (host_ != "") {
        text << "//";
        if (userInfo_ != "") {
            text << Uri::encodeReserved(userInfo_, "/?#[]@!$&'()*+,;=");
        }
        if (host_ != "") {
            text << "@";
            text << Uri::encode(host_);
        }
        if (port_ != -1) {
            text << ":" << str(port_);
        }
    }
    text << Uri::encodeReserved(path_, ":?#[]@!$&'()*+,;=");
    if (query_ != "") {
        text << "?";
        text << Uri::encodeReserved(query_, ":/?#[]@!$&'()*+,;");
    }
    if (fragment_ != "") {
        text << "#";
        text << Uri::encode(fragment_);
    }

    return text;
}

Uri::Uri(const String &text)
{
    UriSyntax syntax;
    me().readUri(syntax, text, syntax.parse(text));
}

Uri::Uri(const String &text, const Token &token)
{
    me().readUri(UriSyntax{}, text, token);
}

String Uri::requestHost() const
{
    return me().requestHost();
}

String Uri::requestPath() const
{
    return me().requestPath();
}

String Uri::toString() const
{
    return me().toString();
}

String Uri::encoded() const
{
    return me().encoded();
}

String Uri::encodeReserved(const String &text, const char *reservedRange)
{
    bool needsEncoding = false;

    for (char ch: text) {
        for (const char *reserved = reservedRange; *reserved; ++reserved) {
            if (ch == *reserved || static_cast<unsigned>(ch) >= 0x80) {
                needsEncoding = true;
                break;
            }
        }
        if (needsEncoding) break;
    }

    if (!needsEncoding) return text;

    String result = text.copy();

    List<String> parts;
    long j = 0;

    for (long i = 0, n = result.count(); i < n; ++i) {
        char ch = result.at(i);
        for (const char *reserved = reservedRange; *reserved; ++reserved) {
            if (ch == *reserved || static_cast<unsigned>(ch) >= 0x80) {
                if (j < i) {
                    parts.append(result.copy(j, i));
                }
                String pct{"%XX"};
                const char *hex = "0123456789abcdef";
                pct.at(1) = hex[(static_cast<unsigned>(ch) >> 4) & 0xF];
                pct.at(2) = hex[(static_cast<unsigned>(ch) & 0xF)];
                parts.append(pct);
                j = i + 1;
                break;
            }
        }
    }

    if (j < result.count()) {
        parts.append(result.copy(j, result.count()));
    }

    return parts;
}

String Uri::decode(const String &text)
{
    if (!text.find('%')) return text;
    String result = text.copy();
    decode(&result);
    return result;
}

void Uri::decode(InOut<String> text)
{
    long j = 0;
    for (long i = 0, n = text->count(); i < n; ++i, ++j) {
        char ch = text->at(i);
        if ((ch == '%') && (i + 2 < n)) {
            unsigned char x = 0;
            char d = text->at(i + 1);
            bool match = true;
            if (('0' <= d) && (d <= '9')) x = d - '0';
            else if (('a' <= d) && (d <= 'f')) x = d - 'a' + 10;
            // else if (('A' <= d) && (d <= 'F')) x = d - 'F' + 10;
            else match = false;
            if (match) {
                x *= 16;
                d = text->at(i + 2);
                if (('0' <= d) && (d <= '9')) x += d - '0';
                else if (('a' <= d) && (d <= 'f')) x += d - 'a' + 10;
                // else if (('A' <= d) && (d <= 'F')) x = d - 'F' + 10;
                else match = false;
                if (match) {
                    i += 2;
                    ch = (char)x;
                }
            }
        }
        if (j < i) text->at(j) = ch;
    }
    text->truncate(j);
}

String Uri::encodeForm(const Map<String> &form)
{
    List<String> parts;
    for (const auto &item: form) {
        parts.append(
            (
                List<String>{}
                << Uri::encode(item.key())
                << Uri::encode(item.value())
            ).join('=')
        );
    }
    return parts.join('&');
}

Map<String> Uri::decodeForm(const String &payload)
{
    Map<String> form;
    auto parts = payload.split('&');
    for (const auto &part: parts) {
        long j = 0;
        if (part.find('=', &j)) {
            String key = Uri::decode(part.copy(0, j));
            String value = Uri::decode(part.copy(j + 1, part.count()));
            form.insert(key, value);
        }
    }
    return form;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Glob>
#include <cc/List>
#include <cc/Dir>
#include <cc/Pattern>

namespace cc {

struct Glob::State: public Object::State
{
    explicit State(const String &text):
        remainder_{text.split('/')}
    {
        if (text.startsWith('/')) {
            remainder_.popFront();
            init("/");
        }
        else {
            init(".");
        }
    }

    State(const String &path, const List<String> &remainder):
        remainder_{remainder}
    {
        init(path);
    }

    void init(const String &path)
    {
        dir_ = Dir{path};
        String text = remainder_.first();
        if (!text.endsWith('^')) text = text + "^";
        pattern_ = text;
        remainder_.popFront();
    }

    bool read(Out<String> path)
    {
        if (child_) {
            if (child_.read(path))
                return true;
            child_.Object::me = nullptr;
        }
        for (String name; dir_.read(&name);) {
            if (name == ".") continue;
            if (name == ".." && pattern_.text() != "..") continue;
            if (pattern_.match(name)) {
                String node = dir_.path() / name;
                if (remainder_.count() == 0) {
                    path = node;
                    return true;
                }
                if (Dir::exists(node)) {
                    child_.Object::me = new State{node, remainder_};
                    return read(path);
                }
            }
        }
        return false;
    }

    Dir dir_;
    Pattern pattern_;
    List<String> remainder_;
    Glob child_;
};

Glob::Glob(const String &text):
    Object{new State{text}}
{}

bool Glob::read(Out<String> path)
{
    return me().read(path);
}

List<String> Glob::explain() const
{
    return me().pattern_.explain();
}

String Glob::text() const
{
    return me().pattern_.text();
}

Glob::State &Glob::me()
{
    return Object::me.as<State>();
}

const Glob::State &Glob::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
