#include "BasicTypes.h"
#include "BitIO.h"
#include "Codec.h"
#include "Lzw.h"

#include <memory>
#include <iostream>
#include <exception>

int main(int argc, char** argv)
{
	std::shared_ptr<Codec> coder(new Lzw());
	BitWriter w(&std::cout);
	BitReader r(&std::cin);

	coder->encode(&r,&w);
	//coder->

	return 0;
}
