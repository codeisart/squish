#include "BitIO.h"
#include "BasicTypes.h"

BitReader::BitReader(const void* mem, size_t size)
	: m_bitBuffer(0)
	, m_currentBitIndex(0)
	, m_memIn(mem)
	, m_memInSize(size)
	, m_memInIndex(0)
	, m_endOfData(false)
{	
	fillBuffer();
}

BitReader::BitReader(const char* filename)
	: m_bitBuffer(0)
	, m_currentBitIndex(0)
	, m_memIn(0)
	, m_memInSize(0)
	, m_memInIndex(0)
	, m_endOfData(false)
{
	m_in.open(filename, std::ifstream::in | std::ifstream::binary);
	//m_in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
	fillBuffer();
}

bool BitReader::fillBuffer()
{
	m_currentBitIndex = 0;

	if( m_memIn )
	{
		m_bitBuffer = static_cast<const u8*>(m_memIn)[m_memInIndex++];
		m_endOfData = m_memInIndex > m_memInSize;
	}
	else
	{
		m_in.read((char*)&m_bitBuffer,sizeof(m_bitBuffer));		
		m_endOfData = m_in.eof();
	}
	return m_endOfData;
}


bool BitWriter::writeBuffer()
{	
	if( m_memOut )
	{
		static_cast<u8*>(m_memOut)[m_memOutIndex++] = m_bitBuffer;		
		m_outOfSpace = m_memOutIndex > m_memOutSize;
	}
	else
		m_out.write((char*)&m_bitBuffer,sizeof(m_bitBuffer));
	
	m_currentBitIndex = 0;
	m_bitBuffer = 0;

	return m_outOfSpace;
}

BitWriter::~BitWriter()
{
	if( m_currentBitIndex != 0 )
		writeBuffer();
}

BitWriter::BitWriter( void* mem, size_t size )
	: m_bitBuffer(0)
	, m_currentBitIndex(0)
	, m_memOut(mem)
	, m_memOutSize(size)
	, m_memOutIndex(0)
	, m_outOfSpace(false)
{
}

BitWriter::BitWriter( const char* filename )
	: m_bitBuffer(0)
	, m_currentBitIndex(0)
	, m_memOut(0)
	, m_memOutSize(0)
	, m_memOutIndex(0)
	, m_outOfSpace(false)
{
	m_out.open(filename, std::ofstream::out | std::ofstream::binary);
	//m_out.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
}

void BitWriter::close()
{
	if( m_currentBitIndex != 0 )
		writeBuffer();

	if( m_out.is_open() )
		m_out.close();
}
