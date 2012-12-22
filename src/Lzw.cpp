#include "Lzw.h"
#include "BitIO.h"

#include <unordered_map>
#include <string>
#include <iostream>

Lzw::Lzw()
{
}

size_t Lzw::decode(BitReader* src, BitWriter* dst)
{
	typedef std::unordered_map<u32,std::string> Codes;

	Codes codes;
	std::string previous;
	u32 code;
	u32 nextCode = FirstCode;
	u32 bitSize = BitSize;

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

size_t Lzw::encode(BitReader* src, BitWriter* dst)
{
	typedef std::unordered_map<std::string,u32> Codes;

	Codes codes;
	std::string current;
	char c;
	u32 nextCode = FirstCode;
	u32 bitSize = BitSize;

	for(u32 i = 0; i < 256; ++i)
		codes[std::string(1,(char)i)] = i;

	while(src->readBits(&c,8))
	{
		current += c;
		if(codes.find(current) == codes.end())
		{
			codes[current] = nextCode++;
			current.erase(current.size()-1);
			dst->writeBits(codes[current],bitSize);
			current = c;
			
			if(nextCode > static_cast<u32>(1 << bitSize)-1)
			{
				dst->writeBits(static_cast<u32>(BumpBitSize), bitSize);
				bitSize++;
			}	
		}
	}

	dst->writeBits(codes[current],bitSize);
	dst->writeBits(static_cast<u32>(EndOfData),bitSize);
	
	return 0;	// FIXME!
}



