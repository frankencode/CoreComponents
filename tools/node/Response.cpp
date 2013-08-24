/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h> // DEBUG
#include <fkit/System.h>
#include <fkit/Date.h>
#include "NodeConfig.h"
#include "ClientConnection.h"
#include "PayloadSink.h"
#include "Response.h"

namespace fnode
{

Ref<Response> Response::create(ClientConnection *client)
{
	return new Response(client);
}

Response::Response(ClientConnection *client)
	: client_(client),
	  headerWritten_(false),
	  statusCode_(200),
	  reasonPhrase_("OK")
{}

const char *reasonPhraseByStatusCode(int statusCode)
{
	const char *phrase = "";
	switch (statusCode) {
	case 100: phrase = "Continue"; break;
	case 101: phrase = "Switching Protocols"; break;
	case 200: phrase = "OK"; break;
	case 201: phrase = "Created"; break;
	case 202: phrase = "Accepted"; break;
	case 203: phrase = "Non-Authoritative Information"; break;
	case 204: phrase = "No Content"; break;
	case 205: phrase = "Reset Content"; break;
	case 206: phrase = "Partial Content"; break;
	case 300: phrase = "Multiple Choices"; break;
	case 301: phrase = "Moved Permanently"; break;
	case 302: phrase = "Moved Temporarily"; break;
	case 303: phrase = "See Other"; break;
	case 304: phrase = "Not Modified"; break;
	case 305: phrase = "Use Proxy"; break;
	case 400: phrase = "Bad Request"; break;
	case 401: phrase = "Unauthorized"; break;
	case 402: phrase = "Payment Required"; break;
	case 403: phrase = "Forbidden"; break;
	case 404: phrase = "Not Found"; break;
	case 405: phrase = "Method Not Allowed"; break;
	case 406: phrase = "Not Acceptable"; break;
	case 407: phrase = "Proxy Authentication Required"; break;
	case 408: phrase = "Request Timeout"; break;
	case 409: phrase = "Conflict"; break;
	case 410: phrase = "Gone"; break;
	case 411: phrase = "Length Required"; break;
	case 412: phrase = "Precondition Failed"; break;
	case 413: phrase = "Request Entity Too Large"; break;
	case 414: phrase = "Request-URI Too Long"; break;
	case 415: phrase = "Unsupported Media Type"; break;
	case 500: phrase = "Internal Server Error"; break;
	case 501: phrase = "Not Implemented"; break;
	case 502: phrase = "Bad Gateway"; break;
	case 503: phrase = "Service Unavailable"; break;
	case 504: phrase = "Gateway Timeout"; break;
	case 505: phrase = "HTTP Version Not Supported"; break;
	}
	return phrase;
}

void Response::status(int statusCode, String reasonPhrase)
{
	statusCode_ = statusCode;
	reasonPhrase_ = reasonPhrase;
	if (reasonPhrase_ == "") reasonPhrase_ = reasonPhraseByStatusCode(statusCode_);
}

void Response::header(String name, String value)
{
	insert(name, value);
}

String httpDate(double time)
{
	Ref<Date> date = Date::create(time);
	const char *dayNames[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	const char *monthNames[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	return Format()
		<< dayNames[date->weekday()] << ", "
		<< dec(date->day(), 2) << " " << monthNames[date->month()] << " " << date->year() << " "
		<< dec(date->hour(), 2) << ":" << dec(date->minute(), 2) << ":" << dec(date->second(), 2) << " GMT";
}

void Response::writeHeader()
{
	Format header(client_->stream());
	header << "HTTP/1.1 " << statusCode_ << " " << reasonPhrase_ << "\r\n";
	establish("Transfer-Encoding", "chunked");
	String now = httpDate(System::now());
	insert("Last-Modified", now);
	insert("Date", now);
	insert("Server", nodeConfig()->version());
	for (int i = 0; i < size(); ++i)
		header << keyAt(i) << ":" << valueAt(i) << "\r\n";
	header << "\r\n";
	ferr() << header->join();

	headerWritten_ = true;
}

void Response::begin()
{
	if (!headerWritten_) writeHeader();
}

PayloadSink *Response::payload()
{
	if (!payload_) {
		if (!headerWritten_) writeHeader();
		payload_ = PayloadSink::open(client_->stream());
	}
	return payload_;
}

void Response::write(String bytes)
{
	payload()->write(bytes);
}

Format Response::chunk(String pattern)
{
	return Format(pattern, payload());
}

Format Response::chunk()
{
	return Format(payload());
}

void Response::end()
{
	payload_ = 0;
}

} // namespace fnode
