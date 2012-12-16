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
	enum { BitSize = 12, EndOfData = 256, BumpBitSize, FlushDictionary, FirstCode};
};

#endif //D_LZW_H
