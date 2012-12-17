#ifndef D_BITIO_H
#define D_BITIO_H

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include "Util.h"
#include "BasicTypes.h"

class BitWriter
{
public:
	BitWriter(std::ostream* out)
		: m_out(out)	
        	, m_bitBuffer(0)
        	, m_currentBitIndex(0)
	{}

	void flush()
	{
		if( m_currentBitIndex != 0 ) 
			writeBuffer(); 
		
		m_out->flush();
		//std::cout << "flush()" << std::endl;
	}

	virtual ~BitWriter() 
	{ 
		flush();
	} 	

	template<typename T> 
	int writeBits(T word, int nBits)
	{
		int i = 0;
		for(; i < nBits; ++i)
		{ 
			T bit = (word >> i) & 0x1;
			m_bitBuffer |= bit << m_currentBitIndex++;

			if( m_currentBitIndex >= 8 )
				writeBuffer();
		}
		return i;
	}

protected:
	bool writeBuffer()
	{     
		m_out->write((char*)&m_bitBuffer,sizeof(m_bitBuffer));
		//std::cout << "writeBuffer() - write: " << static_cast<int>(m_bitBuffer) << std::endl;
		m_currentBitIndex = 0;
		m_bitBuffer = 0;
		return false;
	}

	std::ostream* m_out;
	u8 m_bitBuffer;
	int m_currentBitIndex;
};

class BitStringWriter : public BitWriter
{
private:
	std::ostringstream m_outStream;
public:
	typedef BitWriter Base;
	BitStringWriter(const std::string& initialString)
		: Base(&m_outStream),
		  m_outStream(initialString)
	{}

	std::string getString() const { return m_outStream.str(); }
};

class BitFileWriter : public BitWriter
{
private:
	std::ofstream m_fileStream;
public: 
	typedef BitWriter Base;
	BitFileWriter(const char* filename)
		: Base(&m_fileStream)
	{
	        m_fileStream.open(filename, std::ifstream::in | std::ifstream::binary);
	}

	~BitFileWriter()
	{
		close();
	}

	void close()
	{
		if( m_fileStream.is_open() )
			m_fileStream.close();
	}
};

template<>
inline int BitWriter::writeBits<std::string>(const std::string s, int)
{
	for(std::string::const_iterator i = s.begin(); i != s.end(); ++i)
		writeBits<char>(*i,8);

	return 8 * s.size();
}

class BitReader
{
public:
	BitReader(std::istream* in)
		: m_in(in)
		, m_bitBuffer(0)
		, m_currentBitIndex(0)
		, m_endOfData(false)
	{}

	virtual ~BitReader()
	{}

	template<typename WordSize>
	int readBits(WordSize* word, int nBits);

protected:
	bool fillBuffer();
	
	std::istream *m_in;
	u8 m_bitBuffer;
	int m_currentBitIndex;
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

	//if(m_endOfData)
	//	std::cout << "endOfData" << std::endl;
	
	*out = word;
	
	return i;
}

class BitStringReader : public BitReader
{
private:
	std::istringstream m_inStream;
public:
	typedef BitReader Base;
	BitStringReader(const std::string& s)
		: Base(&m_inStream)
		, m_inStream(s)	
	{
		fillBuffer();
	}
};

class BitFileReader : public BitReader
{
private:
	std::ifstream m_inFile;
public:
	typedef BitReader Base;
	void close()
	{
		if( m_inFile.is_open() )
			m_inFile.close();
	}
	BitFileReader(const char* filename)
		:Base(&m_inFile)
	{
		m_inFile.open(filename, std::ios::in | std::ios::binary);
		fillBuffer();
	}	
	~BitFileReader()
	{
		close();
	}

};


#endif //D_BITIO_
