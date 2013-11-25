#include "LzwFast.h"
#include "BitIO.h"
#include <Primes.h>

#include <string>
#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <cmath>

struct LzwFast::D
{
	int c;
	int previousCode;
	int code;

	D():c(0),previousCode(0),code(-1) {}
};

LzwFast::LzwFast(LzwParams* params)
	: m_tableSize(0)
	, m_table(nullptr)
{
	if( params )
		m_params = *params;
}

LzwFast::~LzwFast()
{
	reset();
}

void LzwFast::reset()
{
	if( m_table )
	{
		delete[] m_table;
		m_table = nullptr;
	}
	m_tableSize = 0;
}

void LzwFast::buildTable(int codeSize)
{
	static const float kExtraSpace = 1.25f; // 25% extra space in table.

	//std::cout << "buildTable " << codeSize << std::endl;
	
	// Backup these.
	int oldSize = m_tableSize;
	D* oldTable = m_table;

	// Code size dictates table size.
	m_tableSize = (1 << codeSize);
	
	//std::cout << "table size to match code size " << m_tableSize << std::endl;
	
	m_tableSize = (int)(ceil( (float)m_tableSize * kExtraSpace ));
	
	//std::cout << "table size before rounding up to prime " << m_tableSize << std::endl;
	
	// Round up to nearest prime number.
	m_tableSize = Primes::findGreaterThan(m_tableSize);

	// Make the new table.
	m_table = new D[m_tableSize];
	
	// Do we already have a table we need to resize?
	if( oldTable )
	{
		// Run through and populate new table.
		for( int i = 0; i < oldSize; ++i )
		{
			const D& e = m_table[i];
			if( e.code != -1 )
			{
				int index = findMatch( e.previousCode, e.c );
				assert( index >= 0 && index < (int)m_tableSize );
				m_table[index] = e;
			}
		}
		
		// Delete old table.
		delete[] oldTable;
	}
}

size_t LzwFast::decode(BitReader* src, BitWriter* dst)
{
	typedef std::unordered_map<u32,std::string> Codes;

	Codes codes;
	std::string previous;    
	u32 code;
	u32 nextCode = FirstCode;
	u32 bitSize = m_params.startingCodeSize;

	//std::cout << "decoding... @ " << src->tell() << std::endl;

	for(u32 i = 0; i < 256; ++i)
		codes[i] = std::string(1,(char)i);

	while(src->readBits(&code,bitSize) && code != EndOfData)
	{
		if( code == BumpBitSize )
		{
			bitSize++;
			continue;
		}	

		if(codes.find(code) == codes.end())
			codes[code] = previous + previous[0];

		dst->writeBits(codes[code],8);

		if(previous.size())
			codes[nextCode++] = previous + codes[code][0];

		previous = codes[code];
	}

	return 0;	// FIXME!
}


//static const int tableSize = 521;
//D g_table[tableSize];

int LzwFast::findMatch(int previousCode, int c)
{
	int hash = (previousCode * 83203) ^ (c * 109717);   // these are two random large primes.
	int index = hash % m_tableSize;
	int attempts = m_tableSize;           				// define this somewhere.
	int offset = index == 0 ? 1 : m_tableSize - index;

	for(int i = 0; i < attempts; ++i)
	{
		if( m_table[index].code == -1 )
		{
			return index;
		}
		else if( m_table[index].previousCode == previousCode &&
				 m_table[index].c == c )
		{
			return index;
		}

		index -= offset;
		if( index < 0 )
			index += m_tableSize;
	}

	return -1;  // Failure, table full.
}

size_t LzwFast::encode(BitReader* src, BitWriter* dst)
{
	int codeSize = m_params.startingCodeSize;
	int nextCode = LzwFast::FirstCode;
	int c;
	int code;

	reset();
	buildTable(codeSize);
	
	src->readBits(&code,8);
	while( src->readBits(&c,8) )
	{
		int index = findMatch(code,c);

		// table full?
		assert(index >= 0 && index < (int)m_tableSize && "failed to find free entry in table");

		//std::cout << "codesize is" << m_codeSize << std::endl;
		
		D& e = m_table[index];
		if( e.code == -1 )	
		{
			// add new code to dict.
			e.c = c;
			e.previousCode = code;
			e.code = nextCode++;
		
			// write old code.
			dst->writeBits(code,codeSize);			

			// new code is the character that broke the sequence.
			code = c;
			
			// bump code size?
			if( nextCode > (1 << codeSize)-1 )
			{	
				dst->writeBits((int)BumpBitSize,codeSize);			
				codeSize++;
				//std::cout << "rebuilding table " << std::endl;
				buildTable(codeSize);
				//std::cout << "bumping codesize to " << m_codeSize << std::endl;
			}
		}
		else
		{
			code = e.code;
		}
	}
	dst->writeBits(code,codeSize);			
	dst->writeBits((int)EndOfData,codeSize);	

	return 0;	// FIXME!
}



