/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Exception>

namespace cc {
namespace zip {

/** \class ZipError exceptions.h cc/zip/exceptions
  * \brief Something went wrong processing a ZIP archive
  */
class ZipError: public Exception
{
public:
    ZipError(int errorCode, String zipPath):
        errorCode_(errorCode),
        zipPath_(zipPath)
    {}
    ~ZipError() throw() {}

    int errorCode() const { return errorCode_; }
    String zipPath() const { return zipPath_; }

    virtual String message() const override;

private:
    int errorCode_;
    String zipPath_;
};

}} // namespace cc::zip
