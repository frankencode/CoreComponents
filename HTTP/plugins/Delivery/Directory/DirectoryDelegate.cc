/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DirectoryDelegate>
#include <cc/DirectoryInstance>
#include <cc/HttpError>
#include <cc/httpDate>
#include <cc/Dir>
#include <cc/File>
#include <cc/FileInfo>
#include <cc/Date>

namespace cc {

struct DirectoryDelegate::State: public HttpServiceDelegate::State
{
    void listDirectory(const HttpRequest &request, const String &path)
    {
        Dir dir{path};

        response().setHeader("Content-Type", "text/html");
        response().chunk() <<
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>" << request.uri() << "</title>\n" <<
            "<style type=\"text/css\">\n"
            "a.file { float: left; width: 100pt; font-size: 10pt; padding: 10pt; overflow: hidden; text-overflow: ellipsis }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n";

        String prefix = request.uri().canonicalPath();

        if (!prefix.endsWith('/')) prefix += "/";

        const bool showHidden = serviceInstance().as<DirectoryInstance>().showHidden();

        List<String> entries;
        for (const String &name: dir) {
            if (!showHidden && name.startsWith('.')) continue;
            entries.append(name);
        }
        entries.sort();

        for (const String &name: entries)
            response().chunk() << "<a class=\"file\" href=\"" << prefix << name << "\">" << name << "</a>\n";

        response().chunk() <<
            "</body>\n"
            "</html>\n";
    }

    void deliverFile(const String &path)
    {
        String content = File{path}.map();
        String mediaType = serviceInstance().mediaTypes().lookup(path, content);
        if (mediaType != "") response().setHeader("Content-Type", mediaType);
        response().beginTransmission(content.count());
        response().write(content);
        response().endTransmission();
    }

    void streamFile(const String &path)
    {
        File file{path};
        String head;
        long long size = -1;
        if (file.isSeekable()) {
            head = file.readSpan(64);
            size = file.seek(0, Seek::End);
            file.seek(0, Seek::Begin);
        }
        String mediaType = serviceInstance().mediaTypes().lookup(path, head);
        if (mediaType != "") response().setHeader("Content-Type", mediaType);
        response().beginTransmission(size);
        file.transferTo(response().payload(), size, Bytes::allocate(0x10000));
        response().endTransmission();
    }

    void process(const HttpRequest &request) override
    {
        DirectoryInstance service = serviceInstance().as<DirectoryInstance>();

        String path = service.path() + "/" + request.uri();
        path = path.canonicalPath();
        String prefix = path.selectHead(service.path().count());
        if (prefix != service.path()) throw HttpForbidden{};

        if (!service.showHidden() && path.baseName().startsWith('.')) throw HttpNotFound{};

        FileInfo fileStatus{path};

        if (!fileStatus) throw HttpNotFound{};

        {
            String h;
            if (request.header().lookup("If-Modified-Since", &h)) {
                Date cacheDate = httpDateFromString(h);
                if (cacheDate) {
                    if (fileStatus.lastModified() <= cacheDate.time()) {
                        response().setStatus(HttpStatus::NotModified);
                        response().beginTransmission();
                        response().endTransmission();
                        return;
                    }
                }
            }
        }

        response().setHeader("Last-Modified", httpDateToString(Date{fileStatus.lastModified()}));

        if (fileStatus.type() == FileType::Directory) {
            String indexPath, indexName;
            const char *candidateNames[] = { "index.html", "index.htm" };
            for (int i = 0, n = sizeof(candidateNames) / sizeof(candidateNames[0]); i < n; ++i) {
                String candidateName = candidateNames[i];
                String candidatePath = path / candidateName;
                if (File::exists(candidatePath)) {
                    indexPath = candidatePath;
                    indexName = candidateName;
                    break;
                }
            }
            if (indexPath != "") {
                if (!request.uri().endsWith('/')) {
                    /* NOTE: Not all user agents (e.g. FireFox) support Content-Location correctly.
                     * Therefore a redirection is the only clean way to make the user agent use a proper full path
                     * in subsequent requests.
                     */
                    response().setStatus(HttpStatus::MovedPermanently);
                    response().setHeader("Location", request.uri() + "/");
                    response().beginTransmission();
                    response().write(String{"Moved permanently."});
                    response().endTransmission();
                    return;
                }

                deliverFile(indexPath);
            }
            else listDirectory(request, path);
        }
        else if (fileStatus.type() == FileType::Regular && fileStatus.size() < 0x10000) {
            deliverFile(path);
        }
        else {
            streamFile(path);
        }
    }
};

DirectoryDelegate::DirectoryDelegate():
    HttpServiceDelegate{new State}
{}

} // namespace cc
