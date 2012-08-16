#ifndef D_BITIO_H
#define D_BITIO_H

#include <fstream>
#include "Util.h"
#include "BasicTypes.h"

class BitWriter
{
public: 
	BitWriter(void* mem, size_t size);
	BitWriter(const char* filename);
	~BitWriter();

	void close();

	template<typename WordSize>
	int writeBits(WordSize word, int nBits);

private:
	bool writeBuffer();

	std::ofstream m_out;
	u8 m_bitBuffer;
	int m_currentBitIndex;

	void* m_memOut;
	size_t m_memOutSize;
	size_t m_memOutIndex;
	bool m_outOfSpace;
};

template<typename WordSize>
inline int BitWriter::writeBits(WordSize word, int nBitsWrite)
{
	int i = 0;
	for(; i < nBitsWrite && !m_outOfSpace; ++i)
	{		
		WordSize bit = (word >> i) & 0x1;	
		m_bitBuffer |= bit << m_currentBitIndex++;	

		if( m_currentBitIndex >= 8 )
			writeBuffer();
	}
	return i;
}

class BitReader
{
public:
	BitReader(const void* mem, size_t size);
	BitReader(const char* filename);	

	template<typename WordSize>
	int readBits(WordSize* word, int nBits);
private:
	bool fillBuffer();
	
	std::ifstream m_in;
	u8 m_bitBuffer;
	int m_currentBitIndex;

	const void* m_memIn;
	size_t m_memInSize;
	size_t m_memInIndex;

	bool m_endOfData;
};


template<typename WordSize>
inline int BitReader::readBits(WordSize* out, int nBitsToRead)
{	
	WordSize word = 0;
	
	int i = 0;
	for(; i < nBitsToRead && !m_endOfData; ++i)
	{
		if( m_currentBitIndex >= 8 )
			if( fillBuffer() )
				break;
	
		WordSize bit = (m_bitBuffer >> m_currentBitIndex++) & 0x1;	
		word |= bit << i;	
	}
	
	*out = word;
	
	return i;
}

#endif //D_BITIO_