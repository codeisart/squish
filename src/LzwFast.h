#pragma once

#ifndef D_LZWFAST_H
#define D_LZWTAST_H

#include "config.h"
#include "BasicTypes.h"
#include "CompilerExtras.h"
#include "Codec.h"
#include <exception>

class BitReader;
class BitWriter;

class LzwFast : public Codec
{
public:
	struct LzwParams
	{
		u32 startingCodeSize;
		LzwParams() : startingCodeSize(12) {}
	};
	LzwFast(LzwParams* Params = nullptr);
	virtual ~LzwFast();

	enum { EndOfData = 256, BumpBitSize, FlushDictionary, FirstCode };
protected:
	virtual size_t encode( BitReader* src, BitWriter* dst ) override;	
	virtual size_t decode( BitReader* src, BitWriter* dst ) override; 

private:
	void reset();
	void buildTable(int codeSize);
	int findMatch(int prev, int c);
	
	LzwParams m_params;
	u32 m_tableSize;
	struct D;
	D* m_table;
};

#endif //D_LZWFAST_H
