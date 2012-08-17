#ifndef D_LZW_H
#define D_LZW_H

#include "BasicTypes.h"
#include "CompilerExtras.h"
#include "Codec.h"
#include <exception>

class BitReader;
class BitWriter;

class Lzw : public Codec
{
public:
	Lzw();

protected:
	virtual size_t encode( BitReader& src, BitWriter& dst ) override;	
	virtual size_t decode( BitReader& src, BitWriter& dst ) override; 

private:
	enum { BitSize = 12, MaxDictSize = 1 << (BitSize+1), Mask = MaxDictSize-1, EndOfData = 256, FlushDictionary = 257, FirstCode = 256, HashingShift=BitSize-8, Available = ~0 };

	struct Node
	{
		Node() : code(Available),precedingStringHash(Available),character(Available) {}
		u32 code;
		u32 precedingStringHash;
		u32 character;
	};

	Node m_dictionary[MaxDictSize];	
	u32 m_nextCode;

	u32 stringToHash(u32 character, u32 precedingStringHash);
	u32 findInDictionary(u32 character, u32 precedingStringHash);
};



#endif //D_LZW_H
