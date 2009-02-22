#ifndef PONA_UTF8SINK_HPP
#define PONA_UTF8SINK_HPP

#include "ByteSink.hpp"

namespace pona
{

class Utf8Sink: public ByteSink
{
public:
	Utf8Sink();
	Utf8Sink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA);
	Utf8Sink(uint8_t* buf, int bufCapa);
	
	void writeChar(uint32_t ch, bool* valid = 0);
};

} // namespace pona

#endif // PONA_UTF8SINK_HPP
