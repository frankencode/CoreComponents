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
#include "ChunkStream.h"
#include "Response.h"

namespace fnode
{

Ref<Response> Response::create(Stream *client)
{
	return new Response(client);
}

Response::Response(Stream *client)
	: client_(client),
	  headerWritten_(false)
{}

Response::~Response()
{
	if (payload_) end();
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
	Format header(client_);
	header << "HTTP/1.1 200 OK\r\n";
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
	payload_ = ChunkStream::open(client_);
}

void Response::write(String bytes)
{
	if (!payload_) begin();
	payload_->write(bytes);
}

Format Response::chunk(String pattern)
{
	if (!payload_) begin();
	return Format(pattern, payload_);
}

Format Response::chunk()
{
	if (!payload_) begin();
	return Format(payload_);
}

void Response::end()
{
	if (!headerWritten_) writeHeader();
	payload_ = 0;
}

} // namespace fnode
