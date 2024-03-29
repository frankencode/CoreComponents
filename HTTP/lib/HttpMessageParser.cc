/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpMessageParser>
#include <cc/TapBuffer>
#include <cc/StreamTap>
#include <cc/LineSource>
#include <cc/TransferLimiter>
#include <cc/input>

namespace cc {

HttpMessageParser::State::State(const Stream &stream):
    httpStream_{stream},
    lineBuffer_{String::allocate(0x999)}
{}

bool HttpMessageParser::State::isPayloadConsumed() const
{
    return httpStream_.isPayloadConsumed();
}

void HttpMessageParser::State::readMessage(Out<HttpMessage> message)
{
    httpStream_.nextHeader();

    message()->header_.deplete();
    message()->payload_ = httpStream_;

    try {
        TransferLimiter limiter{httpStream_, 0x10000};
        LineSource source{limiter, lineBuffer_};

        String line;
        if (!source.read(&line)) throw HttpCloseRequest{};

        onFirstLineReceived(line, *message);

        String name, value;
        List<String> multiValue;
        for (String line; source.read(&line);) {
            if (line == "") {
                break;
            }
            if (line.at(0) == ' ' || line.at(0) == '\t') {
                if (multiValue.count() == 0) {
                    multiValue = List<String>{value};
                }
                line.trim();
                multiValue.append(line);
                continue;
            }
            if (multiValue.count() != 0) {
                message()->header_.establish(name, multiValue.join());
                multiValue.deplete();
            }
            long i = 0;
            if (!line.find(':', &i)) throw HttpBadRequest{};
            name = line.copy(0, i);
            value = line.copy(i + 1, line.count());
            name.trim();
            value.trim();
            if (value != "") {
                message()->header_.establish(name, value);
            }
        }
        if (multiValue.count() != 0) {
            message()->header_.establish(name, multiValue.join());
        }
    }
    catch (InputExhaustion &) {
        throw HttpBadRequest{};
    }

    onHeaderReceived(*message);

    if (message()->header_.value("Transfer-Encoding") == "chunked") {
        httpStream_.nextChunk();
    }
    else {
        int64_t length = 0;
        String h;
        if (message()->header_.lookup("Content-Length", &h)) {
            int ok = 0;
            length = readNumber<int64_t>(h, &ok);
            if (!ok || length < 0) throw HttpBadRequest{};
        }
        httpStream_.nextPayload(length);
    }
}

HttpMessageParser::HttpMessageParser(State *newState):
    Object{newState}
{}

} // namespace cc
