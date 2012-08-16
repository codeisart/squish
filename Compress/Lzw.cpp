#include "Lzw.h"
#include "BitIO.h"

Lzw::Lzw()
	: m_nextCode(FirstCode)
{
}

size_t Lzw::encode(BitReader& src, BitWriter& dst)
{
	u32 character = 0;
	u32 stringHash = 0;

	int bitsRead = src.readBits(&stringHash,8); 
		
	while( bitsRead )
	{				
		bitsRead = src.readBits(&character,8); 
		u32 index = findInDictionary(character,stringHash);

		// Already in table? 
		if( m_dictionary[index].character != Available )
		{			
			stringHash = m_dictionary[index].code;
		}
		else
		{			
			// Write previous code.
			dst.writeBits(stringHash,BitSize);
			
			m_dictionary[index].character = character;
			m_dictionary[index].code = m_nextCode++;
			m_dictionary[index].precedingStringHash = stringHash;

			stringHash = character; 		
		}
	}
	
	return 0;	// FIXME!
}

u32 Lzw::stringToHash(u32 character, u32 precedingStringHash)
{
	u32 hash = (character << HashingShift) ^ precedingStringHash;
	if( hash == 0 )
		hash++;
	
	return hash;
}

u32 Lzw::findInDictionary(u32 character, u32 precedingStringHash)
{
	u32 hash = stringToHash(character,precedingStringHash);
	
	// Max hops protection
	for( int i = 0; i < MaxDictSize; ++i, ++hash )
	{
		// Already in dictionary Match?
		if( m_dictionary[hash].character == character && m_dictionary[hash].precedingStringHash == precedingStringHash )
			return hash;
		else if( m_dictionary[hash].character == Available )
			return hash;
	}
		
	// Fail.
	return 0;
}



