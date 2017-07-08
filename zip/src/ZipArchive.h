/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Folder>
#include <cc/zip/exceptions>

struct zip;
typedef struct zip zip_t;

namespace cc {
namespace zip {

class ZipArchive: public Folder
{
public:
    /** Open a ZIP archive
      * \param path file path of a ZIP archive
      * \return new object instance
      */
    static Ref<ZipArchive> open(String path);

    /// Return the ZIP archive's file path
    String path() const;

    /** Read next file name from the ZIP archive
      * \param name return name of next entry
      * \return false if no more entries, else true
      */
    bool read(String *name) override;

    /** Open a file from this ZIP archive
      * \param name name of the file to open
      * \return new content stream
      */
    virtual Ref<Stream> openFile(String name) override;

private:
    ZipArchive(String path);
    ~ZipArchive();

    String path_;
    zip_t *archive_;
    uint64_t index_;
};

}} // namespace cc::zip
