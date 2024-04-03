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
    if (query_ != "") parts << "?" << Uri::encodeReserved(query_, "/; ");
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
            if (userInfo_ != "") text << "@";
            text << host_;
        }
        if (port_ > 0) {
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
            text << Uri::encodeReserved(userInfo_, "/?#[]@!$&'()*+,;= ");
        }
        if (host_ != "") {
            if (userInfo_ != "")  text << "@";
            text << Uri::encode(host_);
        }
        if (port_ > 0) {
            text << ":" << str(port_);
        }
    }
    text << Uri::encodeReserved(path_, ":?#[]@!$&'()*+,;= ");
    if (query_ != "") {
        text << "?";
        text << Uri::encodeReserved(query_, ":/?#[]@!$&'()*+,; ");
    }
    if (fragment_ != "") {
        text << "#";
        text << Uri::encode(fragment_);
    }

    return text;
}

SocketAddress Uri::State::resolve(int port) const
{
    SocketAddress address;

    if (scheme_ == "local") {
        address = SocketAddress{ProtocolFamily::Local, requestPath()};
    }
    else {
        if (hostIsNumeric_) {
            address = SocketAddress{family(), host_, port_ > 0 ? port_ : port};
        }
        else {
            auto addressList = SocketAddress::queryConnectionInfo(host_);
            if (addressList.count() > 0) {
                address = addressList.at(0);
                address.setPort(port_ > 0 ? port_ : port);
            }
            else throw HostNameResolutionError{host_};
        }
    }

    return address;
}

ProtocolFamily Uri::State::family() const
{
    if (!hostIsNumeric_) return ProtocolFamily::Unspec;
    return host_.contains(':') ? ProtocolFamily::InternetV6 : ProtocolFamily::InternetV4;
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
            else if (('A' <= d) && (d <= 'F')) x = d - 'A' + 10;
            else match = false;
            if (match) {
                x *= 16;
                d = text->at(i + 2);
                if (('0' <= d) && (d <= '9')) x += d - '0';
                else if (('a' <= d) && (d <= 'f')) x += d - 'a' + 10;
                else if (('A' <= d) && (d <= 'F')) x += d - 'A' + 10;
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
