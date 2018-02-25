/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Dir>
#include <cc/http/utils>
#include <cc/http/exceptions>
#include "ServiceWorker.h"
#include "ErrorLog.h"
#include "DirectoryInstance.h"
#include "MediaTypeDatabase.h"
#include "DirectoryDelegate.h"

namespace ccnode {

using namespace cc::http;

Ref<DirectoryDelegate> DirectoryDelegate::create(ServiceWorker *worker, ScriptHandler *scriptHandler)
{
    return new DirectoryDelegate(worker, scriptHandler);
}

DirectoryDelegate::DirectoryDelegate(ServiceWorker *worker, ScriptHandler *scriptHandler):
    ServiceDelegate(worker),
    directoryInstance_(worker->serviceInstance()),
    scriptHandler_(scriptHandler)
{}

void DirectoryDelegate::process(HttpRequest *request)
{
    String path = directoryInstance_->path() + "/" + request->uri();
    path = path->canonicalPath();
    String prefix = path->head(directoryInstance_->path()->count());
    if (path->head(directoryInstance_->path()->count()) != directoryInstance_->path()) throw Forbidden();

    if ((!directoryInstance_->showHidden()) && path->baseName()->beginsWith('.')) throw NotFound();

    Ref<FileStatus> fileStatus = FileStatus::read(path);

    if (scriptHandler_) {
        if (
            (!fileStatus->isValid()) ||
            ((fileStatus->mode() & AnyExec) && (fileStatus->type() != FileType::Directory))
        ) {
            if (scriptHandler_->process(request, fileStatus, directoryInstance_->path()))
                return;
        }
    }

    if (!fileStatus->isValid()) throw NotFound();

    {
        String h;
        if (request->lookup("If-Modified-Since", &h)) {
            Ref<Date> cacheDate = scanDate(h);
            if (cacheDate) {
                if (fileStatus->lastModified() <= cacheDate->time()) {
                    response()->setStatus(304);
                    response()->beginTransmission();
                    response()->endTransmission();
                    return;
                }
            }
        }
    }

    response()->setHeader("Last-Modified", formatDate(Date::breakdown(fileStatus->lastModified())));

    if (fileStatus->type() == FileType::Directory) {
        String indexPath, indexName;
        const char *candidateNames[] = { "index.html", "index.htm" };
        for (int i = 0, n = sizeof(candidateNames) / sizeof(candidateNames[0]); i < n; ++i) {
            String candidateName = candidateNames[i];
            String candidatePath = path->expandPath(candidateName);
            if (File::exists(candidatePath)) {
                indexPath = candidatePath;
                indexName = candidateName;
                break;
            }
        }
        if (indexPath != "") {
            if (!request->uri()->endsWith('/')) {
                // NOTE: not all user agents (e.g. FireFox) support Content-Location correctly
                // therefore a redirection is the only clean way to make the user agent us a proper full path
                // in subsequent requests
                response()->setStatus(301);
                response()->setHeader("Location", request->uri() + "/");
                response()->beginTransmission();
                response()->write("Moved permanently.");
                response()->endTransmission();
                return;
            }

            deliverFile(indexPath);
        }
        else listDirectory(request, path);
    }
    else if (fileStatus->type() == FileType::Regular && fileStatus->size() < 0x10000) {
        deliverFile(path);
    }
    else {
        streamFile(path);
    }
}

void DirectoryDelegate::listDirectory(HttpRequest *request, String path)
{
    Ref<Dir> dir = Dir::open(path);

    response()->setHeader("Content-Type", "text/html");
    response()->chunk() <<
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>" << request->uri() << "</title>\n" <<
        "<style type=\"text/css\">\n"
        "a.file { float: left; width: 100pt; font-size: 10pt; padding: 10pt; overflow: hidden; text-overflow: ellipsis }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n";

    String prefix = request->uri()->canonicalPath();
    if (prefix->count() > 0) {
        if (prefix->at(prefix->count() - 1) != '/')
            prefix += "/";
    }

    Ref<StringList> entries = StringList::create();
    for (String name; dir->read(&name);) {
        if (name == "." || name == "..") continue;
        if ((!directoryInstance_->showHidden()) && name->beginsWith('.')) continue;
        entries->append(name);
    }
    entries = entries->sort();

    for (int i = 0; i < entries->count(); ++i) {
        String name = entries->at(i);
        response()->chunk() << "<a class=\"file\" href=\"" << prefix << name << "\">" << name << "</a>\n";
    }

    response()->chunk() <<
        "</body>\n"
        "</html>\n";
}

void DirectoryDelegate::deliverFile(String path)
{
    String content = File::open(path)->map();
    String mediaType = mediaTypeDatabase()->lookup(path, content);
    if (mediaType != "") response()->setHeader("Content-Type", mediaType);
    response()->beginTransmission(content->count());
    response()->write(content);
    response()->endTransmission();
}

void DirectoryDelegate::streamFile(String path)
{
    Ref<File> file = File::open(path);
    String head;
    ssize_t size = -1;
    if (file->seekable()) {
        head = file->readSpan(64);
        size = file->seek(0, SeekEnd);
        file->seek(0, SeekBegin);
    }
    String mediaType = mediaTypeDatabase()->lookup(path, head);
    if (mediaType != "") response()->setHeader("Content-Type", mediaType);
    response()->beginTransmission(size);
    Ref<ByteArray> buf = ByteArray::allocate(0x10000);
    while (true) {
        int n = file->read(mutate(buf));
        if (n == 0) break;
        response()->write(buf->select(0, n));
    }
    response()->endTransmission();
}

} // namespace ccnode
