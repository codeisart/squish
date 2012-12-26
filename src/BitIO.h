#ifndef D_BITIO_H
#define D_BITIO_H

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include "Util.h"
#include "BasicTypes.h"
#include "DebugLog.h"

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
	}

	virtual ~BitWriter() 
	{ 
		flush();
	} 	

	template<typename T> 
	int writeBits(T word, int nBits=sizeof(T)*8)
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

	template<typename T> 
	bool write(T word)
	{
		return writeBits(word) == sizeof(T)*8;
	}

	void seek(std::ios::streamoff offset)
	{
		flush();
		m_out->seekp(offset, std::ios::beg);
	}

	std::ios::streamoff tell() const
	{
		std::ios::streamoff off = m_out->tellp();
		
		// If we are half way through a byte round up.
		if(m_currentBitIndex>0)
			off++;
		return off;
	}

protected:
	bool writeBuffer()
	{     
		m_out->write((char*)&m_bitBuffer,sizeof(m_bitBuffer));
		m_currentBitIndex = 0;
		m_bitBuffer = 0;
		return false;
	}

	typedef u8 BitBufferType;

	std::ostream* m_out;
	BitBufferType m_bitBuffer;
	int m_currentBitIndex;
};

template<>
inline int BitWriter::writeBits<std::string>(const std::string s, int)
{
	for(std::string::const_iterator i = s.begin(); i != s.end(); ++i)
		writeBits<char>(*i,8);

	return 8 * s.size();
}

template<>
inline bool BitWriter::write<std::string>(std::string s)
{
	// Write NULL terminated string object.
	for(auto i:s)
		write(i);
	write('\0');
	return true;
}

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
	        m_fileStream.open(filename, std::ios::out | std::ios::binary);
	}

	~BitFileWriter()
	{
		flush();
		close();
	}

	void close()
	{
		if( m_fileStream.is_open() )
			m_fileStream.close();
	}
};


class BitReader
{
public:
	BitReader(std::istream* in)
		: m_in(in)
		, m_bitBuffer(0)
		, m_currentBitIndex(0)
		, m_endOfData(false)
		, m_lastValidTell(0)
	{}

	virtual ~BitReader()
	{}

	template<typename WordSize>
	int readBits(WordSize* word, int nBits=sizeof(WordSize)*8);

	template<typename WordSize>
	bool read(WordSize* word)
	{
		int nBits = readBits(word);
		return nBits == sizeof(WordSize)*8;
	}
	
	bool endOfData() { return m_endOfData; }

	void seek(std::ios::streamoff offset)
	{
		m_in->clear();
		m_in->seekg(offset,std::ios::beg);
		fillBuffer();
	}

	std::ios::streamoff tell() const
	{	
		// If we are half way through a byte round up.
		//if(m_currentBitIndex>0)
		//	return m_lastValidTell+1;

		return m_lastValidTell;
	}

	void roundUp()
	{
		if(m_currentBitIndex)
			fillBuffer();
	}

protected:
	bool fillBuffer();
	
	std::istream *m_in;
	u8 m_bitBuffer;
	int m_currentBitIndex;
	bool m_endOfData;
	std::ios::streamoff m_lastValidTell;
};

inline bool BitReader::fillBuffer()
{
	// Cache the position
	std::ios::streamoff oldPos = m_in->tellg();

	m_currentBitIndex = 0;
	m_in->read((char*)&m_bitBuffer,sizeof(m_bitBuffer));		
	m_endOfData = m_in->eof();

	// Remember the last valid position we were on. 
	std::ios::streamoff newPos = m_in->tellg();
	if(newPos!=-1)
		m_lastValidTell = newPos;
	else if(oldPos != -1)
		m_lastValidTell = oldPos+1;	

	//std::cout << "gtell:" << tell() << " eof=" << m_endOfData << std::endl;
	//std::cout << "fillBuffer - " << static_cast<int>(m_bitBuffer) << ", eof=" << m_endOfData << std::endl;

	return m_endOfData;
}

template<>
inline bool BitReader::read<std::string>(std::string* str)
{
	// Zero out the string to begin with.
	str->clear();

	// Read NULL terminated string.
	char c;
	while(read(&c) && c)
		str->push_back(c);	

	return true;
}

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
