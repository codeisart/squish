#ifndef D_CODEC_H
#define D_CODEC_H

class BitReader;
class BitWriter;

class Codec
{
public:
	virtual size_t encode(BitReader& src, BitWriter& dst) = 0;
	virtual size_t decode(BitReader& src, BitWriter& dst) = 0;
};

#endif //D_CODEC_H