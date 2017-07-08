/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h> // mode_t
#include <dirent.h> // DIR, opendir, closedir, readdir_r
#include <cc/Folder>

namespace cc {

/** \class Dir Dir.h cc/Dir
  * \ingroup file_system
  * \brief Work with directory files
  * \see File, CleanupGuard, DirWalker
  */
class Dir: public Folder
{
public:
    /** Open an existing directory
      * \param path directory file path
      * \return new object instance
      */
    static Ref<Dir> open(String path) { return new Dir(path); }

    /** Open an existing directory, if exists
      * \param path directory file path
      * \return new object instance or null if directory does not exists
      */
    static Ref<Dir> tryOpen(String path);

    /** Open a new tempory directory
      * \return new object instance
      */
    static Ref<Dir> openTemp();

    /// Return the directory's file path
    String path() const;

    /** Read next directory entry
      * \param name return name of next entry
      * \return false if no more entries, else true
      */
    bool read(String *name) override;

    /** Open a file from this directory
      * \param path file path of the file to open
      * \return new content stream
      */
    Ref<Stream> openFile(String path) override;

    /** Check access permissions
      * \param path directory file path
      * \param flags a combination of cc::Access
      * \return true if the calling process has needed credentials
      */
    static bool access(String path, int flags);

    /** Check if directory exists
      * \param path directory file path
      * \return true if a directory exists under given path
      */
    static bool exists(String path);

    /** Count number of files in directory
      * \param path directory file path
      * \return 0 if there is no such directory, otherwise number of files
      */
    static int count(String path);

    /** Create a new directory
      * \param path directory file path
      * \param mode a combination of cc::Mode
      */
    static void create(String path, int mode = 0755);

    /** Establish a new directory path
      * \param path directory file path
      * \param mode a combination of cc::Mode
      */
    static void establish(String path, int mode = 0755);

    /** Remove directory
      * \param path directory file path
      */
    static void remove(String path);

    /** Create a uniquely named directory
      * \param path directory file path pattern
      * \param mode a combination of cc::Mode
      * \param placeHolder place holder character in path to replace with random characters
      * \return name of the newly created directory
      */
    static String createUnique(String path, int mode = 0755, char placeHolder = 'X');

    /** Cleanup directory recursively
      * \param path directory file path
      * Cleans up the given directory path recursively deleting all containing files and directories in the process.
      */
    static void cleanup(String path);

private:
    Dir(String path, DIR *dir = 0);
    ~Dir();

    String path_;
    DIR *dir_;
};

} // namespace cc
