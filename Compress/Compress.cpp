// Compress.cpp : Defines the entry point for the console application.
//

#include "BasicTypes.h"
#include "BitIO.h"
#include "Codec.h"
#include "Lzw.h"

#include <iostream>
#include <exception>

bool test1()
{
	std::cout << "Test 1 - BitReader read hello file one bit at a time until end." << std::endl;
		
	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	for( int strIndex = 0; strIndex < size; ++strIndex )
	{
		for(int i = 0; i < 8; ++i)
		{
			u8 word = 0;
			r.readBits(&word,1);
			u8 expected = (helloStr[strIndex] >> i) & 0x1;
					
			if( word != expected )
			{
				std::cout << 
					"\t" <<
					"Failed at bit: " << i  <<
					" on character: " << strIndex <<
					" expected: " << (int)expected <<
					" read:	" << (int)word <<
					std::endl;

				success = false;
			}
		}	
	}

	// Pass.
	return success;
}

bool test2()
{
	std::cout << "Test 2 - BitReader read hello file 2 bits at a time until end." << std::endl;

	const char* helloStr = "hello";
	const u8 expectedOutput[] = 
		{0,2,2,1,		// h
		1,1,2,1,		// e
		0,3,2,1,  		// l
		0,3,2,1,  		// l
		3,3,2,1			// o
	};	

	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	int expectedIndex = 0;
	for( int strIndex = 0; strIndex < size; ++strIndex )
	{
		for(int i = 0; i < 8; i+=2)
		{
			u8 word = 0;
			r.readBits(&word,2);

			u8 expected = (helloStr[strIndex] >> i) & 0x3

				;//= expectedOutput[expectedIndex++];

			if( word != expected )
			{
				std::cout << 
					"\t" <<
					"Failed at bits: " << i << " and " << i+1 <<
					" on character: " << strIndex <<
					" expected: " << (int)expected <<
					" read:	" << (int)word <<
					std::endl;

				success = false;
			}
		}	
	}

	// Pass.
	return success;
}


bool test3()
{
	std::cout << "Test 3 - BitReader read hello file 3 bits at a time until the end" << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	int expectedIndex = 0;

	int totalBits = 8 * size;
	for( int i = 0; i < totalBits; i+=3)
	{		
		u8 word = 0;
		r.readBits(&word,3);

		u8 expected = 0;
		for( int j = 0; j < 3; j++)	
		{
			int bitIndex = (i+j) % 8;
			int byteIndex = (i+j) / 8;
			u8 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;
			expected |= bit << j;
		}

		if( word != expected )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: [" << i << "][3]" << std::endl;

			success = false;
		}
	}

	// Pass.
	return success;
}

bool test4()
{
	std::cout << "Test 4 - BitReader read in memory 3 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r(helloStr,size);

	bool success = true;
	int expectedIndex = 0;

	int totalBits = 8 * size;
	for( int i = 0; i < totalBits; i+=3)
	{						
		u8 word = 0;
		r.readBits(&word,3);

		u8 expected = 0;
		for( int j = 0; j < 3; j++)	
		{
			int bitIndex = (i+j) % 8;
			int byteIndex = (i+j) / 8;
			u8 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;
			expected |= bit << j;
		}

		if( word != expected )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: [" << i << "][3]" << std::endl;

			success = false;
		}
	}

	// Pass.
	return success;
}


bool test5()
{
	std::cout << "Test 5 - BitReader read in hello text file 8 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	int expectedIndex = 0;

	int totalBits = 8 * size;
	int bitsRead = 0;
	for( int i = 0; i < totalBits; i+=8)
	{						
		u8 word = 0;
		bitsRead += r.readBits(&word,8);

		u8 expected = 0;
		for( int j = 0; j < 8; j++)	
		{
			int bitIndex = (i+j) % 8;
			int byteIndex = (i+j) / 8;
			u8 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;
			expected |= bit << j;
		}

		if( word != expected )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: [" << i << "][8]" << std::endl;

			success = false;
		}
	}

	if( bitsRead != totalBits )
	{
		std::cout << 
			"\t" <<
			"Bits read not equal to total bits: [" << bitsRead << "][" << totalBits << "]" << std::endl;

		success = false;
	}

	// Pass.
	return success;
}

bool test6()
{
	std::cout << "Test 6 - BitReader read in hello text file 12 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	int expectedIndex = 0;

	int totalBits = 8 * size;
	for( int i = 0; i < totalBits; i+=12)
	{						
		u8 word = 0;
		r.readBits(&word,12);

		u8 expected = 0;
		for( int j = 0; j <12; j++)	
		{
			int bitIndex = (i+j) % 8;
			int byteIndex = (i+j) / 8;
			u8 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;
			expected |= bit << j;
		}

		if( word != expected )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: [" << i << "][12]" << std::endl;

			success = false;
		}
	}

	// Pass.
	return success;
}

bool test7()
{
	std::cout << "Test 7 - BitReader read in hello text file 12 bits until EOF " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitReader r("../Compress/Tests/hello.txt");

	bool success = true;
	int expectedIndex = 0;

	int totalBits = 8 * size;
	
	u32 word = 0;
	int i = 0;
		
	while( int bitsRead = r.readBits(&word,12) )
	{		
		u32 expected = 0;
		for( int j = 0; j <12; j++)	
		{	
			int sum = i+j;
			if( sum < totalBits )
			{
				int bitIndex = sum % 8;
				int byteIndex = sum / 8;
				u32 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;
				expected |= bit << j;
			}			
		}

		if( word != expected )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: [" << i << "][12]" << std::endl;

			success = false;
		}
		i += bitsRead;
	}

	// Pass.
	return success;
}


bool test8()
{
	std::cout << "Test 8 - BitWriter write hello 1 bit at a time into a text file, then using BitReader to read it back." << std::endl;

	bool success = true;

	const char* helloStr = "hello";
	int size = strlen(helloStr);
	int totalBits = 8 * size;

	{
		BitWriter w("../Compress/Tests/hello.out");

		for( int i = 0; i < totalBits; ++i)
		{
			int bitIndex = (i) % 8;
			int byteIndex = (i) / 8;
			u32 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;		
			w.writeBits(bit,1);
		}
	}

	{
		BitReader r("../Compress/Tests/hello.out");

		for( int i = 0; i < totalBits; ++i)
		{
			int bitIndex = (i) % 8;
			int byteIndex = (i) / 8;
			u32 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;		
			u32 expected = bit;
			
			u32 word = 0;
			r.readBits(&word,1);

			if( word != expected )
			{
				std::cout << 
					"\t" <<
					"Failed at bit: " << i << std::endl;

				success = false;
			}

		}
	}

	// Pass.
	return success;
}

bool nBitsInAndOutHelloTest(int bitWordSize)
{
	const char* helloStr = "hello";
	int size = strlen(helloStr);
	int totalBits = 8 * size;
	bool success = true;

	{
		BitWriter w("../Compress/Tests/hello.out");

		for( int i = 0; i < totalBits; i+=bitWordSize)
		{
			u32 expected = 0;
			for(int j = 0; j < bitWordSize; j++ )
			{
				int bitIndex = (i+j) % 8;
				int byteIndex = (i+j) / 8;
				u32 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;	
				expected |= bit << j;
			}
			w.writeBits(expected,bitWordSize);
		}
	}

	{
		BitReader r1("../Compress/Tests/hello.out");
		BitReader r2("../Compress/Tests/hello.txt");

		for( int i = 0; i < totalBits; i+=bitWordSize)
		{
			u32 word1 = 0;
			u32 word2 = 0;
			r1.readBits(&word1,bitWordSize);
			r2.readBits(&word2,bitWordSize);
			
			if( word1 != word2 )
			{
				std::cout << 
					"\t" <<
					"Failed at bits: " << i << std::endl;

				success = false;
			}

		}
	}
	return success;
}


bool test9()
{
	std::cout << "Test 9 - BitWriter write hello 2 bits at a time into a text file, then using BitReader to read it back." << std::endl;

	// Pass.
	return nBitsInAndOutHelloTest(2);
}
bool test10()
{
	std::cout << "Test 10 - BitWriter write hello 3 bits at a time into a text file, then using BitReader to read it back." << std::endl;

	// Pass.
	return nBitsInAndOutHelloTest(3);
}
bool test11()
{
	std::cout << "Test 11 - BitWriter write hello 12 bits at a time into a text file, then using BitReader to read it back." << std::endl;

	// Pass.
	return nBitsInAndOutHelloTest(12);
}

bool test12()
{
	std::cout << "Test 12 - BitWriter write hello 8 bits at a time into a text file, then using BitReader to read it back." << std::endl;

	// Pass.
	return nBitsInAndOutHelloTest(8);
};

bool test13()
{
	std::cout << "Test 13 - BitWriter write 1 666 times into a file, then use BitReader to read it back." << std::endl;
	
	bool success = true;
	
	int bitCount = 666;

	{
		BitWriter w("../Compress/Tests/ones_666.dat");
		for( int i = 0; i < bitCount; ++i )
			w.writeBits(0x1,1);
	}
	{
		BitReader r("../Compress/Tests/ones_666.dat");
		for( int i = 0; i < bitCount; ++i)
		{
			u32 word = 0;
			r.readBits(&word,1);
			
			if( word != 1 )
			{
				std::cout << 
					"\t" <<
					"Failed at bits: " << i << std::endl;

				success = false;
			}

		}
	}
	return success;
}


bool test14()
{
	std::cout << "Test 14 - BitWriter write 1 666 times into memory, then use BitReader to read it back." << std::endl;

	bool success = true;

	int bitCount = 666;
	int memSize = 700;
	
	u8* mem = new u8[memSize];
	BitWriter w(mem,memSize);
	for( int i = 0; i < bitCount; ++i )
		w.writeBits(0x1,1);

	w.close();

	BitReader r(mem,memSize);

	for( int i = 0; i < bitCount; ++i)
	{
		u32 word = 0;
		r.readBits(&word,1);

		if( word != 1 )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: " << i << std::endl;

			success = false;
		}

	}

	return success;
}

bool test15()
{
	std::cout << "Test 15 - Compress LZW example strings from Mark Nelson" << std::endl;
	
	bool success = true;
	
	

	const char* text = "/WED/WE/WEE/WEB/WET";
	u32 expectedOut[] = { '/','W','E','D',256,'E',260,261,257,'B',260,'T' };
	size_t expectedSize = sizeof(expectedOut) / sizeof(u32);
	
	{
		Codec* codec = new Lzw();
		BitReader r(text, strlen(text));
		BitWriter w("../Compress/Tests/Lzw.out");
		codec->encode(r,w);
		delete codec;
	}

	BitReader r("../Compress/Tests/Lzw.out");
	u32 word = 0;
	int expectedIndex = 0;
	while( int bitsRead = r.readBits(&word,12) )
	{
		if( bitsRead != 12 )
		{
			std::cout << 
			"\t" <<
			"Expected 12 bits, but read only " << bitsRead << std::endl;	
			success = false;
		}
		if( expectedOut[expectedIndex] != word )
		{
			std::cout << 
				"\t" <<
				"Expected to read " << expectedOut[expectedIndex] << 
				" but read "  << word << std::endl;	
			success = false;
		}
		expectedIndex++;
	}


	return success;
}

bool test16()
{
	std::cout << "Test 16 - BitReader Test that we return the correct number of bits read" << std::endl;

	BitReader r("../Compress/Tests/hello.txt");
	
	bool success = true;
	u32 word = 0;
	while( int bitsRead = r.readBits(&word,8) )
	{
		if( bitsRead != 8 )
		{
			std::cout << 
				"\t" <<
				"Failed to read 8 bits: " << std::endl;		
			success = false;
		}
	}
	return success;
}


bool runTests()
{
	bool success = true;

	try
	{
		success &= test1();
		success &= test2();
		success &= test3();
		success &= test4();
		success &= test5();
		success &= test6();
		success &= test7();
		success &= test8();
		success &= test9();
		success &= test10();
		success &= test11();
		success &= test12();
		success &= test13();
		success &= test14();
		success &= test15();
		success &= test16();
	}
	catch (std::bad_alloc* e)
	{
		std::cout << "Memory exception thrown." << e->what() << std::endl;
	}
	catch (std::exception* e)
	{
		std::cout << "General exception thrown: " << e->what() << std::endl;
	}
	
	return success;
}

#ifdef _MSC_VER
int _tmain(int argc, _TCHAR* argv[])
#else 
int main(int, char** argv)
#endif 
{
	runTests();
	return 0;
}

