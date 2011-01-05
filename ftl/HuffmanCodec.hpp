/*
 * HuffmanCodec.hpp -- block-wise two-pass Huffman codec
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_HUFFMANCODEC_HPP
#define FTL_HUFFMANCODEC_HPP

#include "atoms"
#include "BitEncoder.hpp"
#include "BitDecoder.hpp"
#include "Heap.hpp"
#include "Stack.hpp"

namespace ftl
{

/** \brief Code table entry
  */
class SymbolNode
{
public:
	SymbolNode* parent;
	SymbolNode* leftChild;
	SymbolNode* rightChild;
	int count;
	int value;
};

/** \brief Comparable reference to a code table entry
  */
class SymbolRef
{
public:
	SymbolRef()
		: symbol(0)
	{}

	SymbolRef(SymbolNode* symbol)
		: symbol(symbol)
	{}

	SymbolNode* symbol;

	inline bool operator<(const SymbolRef& b) const
	{
		return symbol->count < b.symbol->count;
	}
};

/** \brief Block-wise two-pass Huffman codec
  *
  * The HuffmanCodec allows to encode any series of integers.
  */
class HuffmanCodec: public Instance
{
public:
	/** Initialize the codec and allocate required memory.
	  *
	  * The arguments provided determine the memory consumption of the codec.
	  * Much more restricted values may be choosen than the default
	  * If the raw diversity or the raw dynamic range is two small
	  * for a given input buffer the codec will fall back to a special
	  * raw transmission mode.
	  *
	  * \param rawDiversity estimated size of the raw symbols alphabet
	  * \param rawDynamicRange estimated dynamic range of raw symbols (max-min+1)
	  */
	HuffmanCodec( int rawDiversity = 0x10000,
	              int rawDynamicRange = 0x10000 );

	/** Free memory.
	  */
	~HuffmanCodec();
	
	/** Returns the number bytes allocated on heap.
      */
	int memoryConsumption() const;

	/** Returns the number of bytes written on encoding in the worst case.
	  * Allows to dimension the output buffer for encoding and the input buffer
	  * for decoding.
	  *
	  * \param rawCapacity maximum size of an input buffer (in symbols)
	  * \param rawDynamicRange maximum dynamic range of raw symbols
	  * \return maximum number of bytes of encoded result
	  */
	int encodedCapacity(int rawCapacity, int rawDynamicRange) const;

	/** Read symbols from an input buffer and write the encoded result
	  * to a bit sink.
	  *
	  * \param sink bit sink
	  * \param raw input buffer
	  * \param rawFill number of symbols to be read from input buffer
	  * \param userFallback if not null returns raw transmission fallback indicator
	  */
	void encode( BitEncoder* sink,
	             int* raw,
	             int rawFill,
	             bool* userFallback = 0 );

	/** Read a complete encoded frame from a bit source and write the decoded
	  * result to an ouput buffer.
	  *
	  * \param raw output buffer
	  * \param rawCapacity size of the output buffer in number of symbols
	  * \param source bit source
	  * \return number of symbols written to the output buffer
	  */
	int decode( int* raw,
	            int rawCapacity,
	            BitDecoder* source );

	/** Read symbols from an input buffer and write the encoded result
	  * to an output buffer.
	  *
	  * \param encoded output buffer
	  * \param encodedCapacity size of the output buffer in number of bytes
	  * \param raw input buffer
	  * \param rawFill number of symbols to be read from input buffer
	  * \param userFallback if not null returns raw transmission fallback indicator
	  * \return number of bytes written to the output buffer
	  */
	int encode( uint8_t* encoded,
	            int encodedCapacity,
	            int* raw,
	            int rawFill,
	            bool* userFallback = 0 );

	/** Read an encoded buffer and write the decoded result to an output buffer.
	  *
	  * \param raw output buffer
	  * \param rawCapacity size of the output buffer in number of symbols
	  * \param encoded input buffer
	  * \param encodedFill number of bytes to be read from input buffer
	  * \return number of symbols written to the output buffer
	  */
	int decode( int* raw,
	            int rawCapacity,
	            uint8_t* encoded,
	            int encodedFill );
	
private:
	void reset();
	void addSymbol(int x, int count0);

	void generateCodeTable();
	void debugCodeTable();
	void writeRawFrame(BitEncoder* sink, int* frame, int frameFill, int rawMin, int rawMax);

	int options_;
	int rawDiversity_;
	int rawDynamicRange_;
	
	SymbolNode* codeTable_;
	SymbolNode* codeTableRoot_;
	int codeTableFill_;

	int codeMapSize_;
	SymbolRef* codeMap_;
	MaxHeap<SymbolRef> heap_;
	Stack<uint8_t> bitStack_;

	inline static int ilog2(int n)
	{
		int i = 0;
		int h = 1;
		while (h < n)
		{
			h *= 2;
			++i;
		}
		return i;
	}
};

} // namespace ftl

#endif // FTL_HUFFMANCODEC_HPP
