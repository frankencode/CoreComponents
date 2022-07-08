/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DispatchLoggingInstance>
#include <cc/HttpLoggingServiceRegistry>

namespace cc {

struct DispatchLoggingInstance::State: public HttpLoggingServiceInstance::State
{
    State(const MetaObject &config):
        HttpLoggingServiceInstance::State{config}
    {
        for (const MetaObject &child: config.children()) {
            HttpLoggingService service = HttpLoggingServiceRegistry{}.serviceByName(child.className());
            targets_.append(service.createInstance(child));
        }
    }

    void logDelivery(const HttpClientConnection &client, const HttpRequest &request, HttpStatus status, long long bytesWritten, const String &statusMessage) const override
    {
        for (const HttpLoggingServiceInstance &target: targets_) {
            target.logDelivery(client, request, status, bytesWritten, statusMessage);
        }
    }

    void logMessage(const String &message, LoggingLevel level) const override
    {
        if (verbosity() >= level) {
            for (const HttpLoggingServiceInstance &target: targets_) {
                target.logMessage(message, level);
            }
        }
    }

    List<HttpLoggingServiceInstance> targets_;
};

DispatchLoggingInstance::DispatchLoggingInstance(const MetaObject &config):
    HttpLoggingServiceInstance{new State{config}}
{}

} // namespace cc:
