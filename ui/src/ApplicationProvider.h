/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Registration>
#include <cc/ui/Application>

namespace cc {
namespace ui {

class ApplicationProvider: public Object
{
public:
    virtual Ref<Application> createApplication(int argc, char **argv) = 0;

protected:
    ApplicationProvider();
};


template<class ApplicationType>
class ApplicationProviderTemplate: public ApplicationProvider
{
public:
    Ref<Application> createApplication(int argc, char **argv) { return ApplicationType::create(argc, argv); }
};

#define CCUI_APPLICATION_REGISTRATION(Type) \
    namespace { Registration< ApplicationProviderTemplate<Type> > registrationOf ## Type; }

}} // namespace cc::ui

