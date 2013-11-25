// Compress.cpp : Defines the entry point for the console application.
//

#include "config.h"
#include "BasicTypes.h"
#include "BitIO.h"
#include "Codec.h"
#include "LzwFast.h"
#include "DebugLog.h"
#include "Archive.h"

#include <memory>
#include <algorithm>
#include <iostream>
#include <exception>

#define BOOST_TEST_MODULE SquishTests
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SquishTests)


BOOST_AUTO_TEST_CASE(test1)
{
	std::cout << "Test 1 - BitFileReader read hello file one bit at a time until end." << std::endl;
		
	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

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
	//BOOST_CHECK(success);
	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test2)
{
	std::cout << "Test 2 - BitFileReader read hello file 2 bits at a time until end." << std::endl;

	const char* helloStr = "hello";
	//const u8 expectedOutput[] = 
		//{0,2,2,1,		// h
		//1,1,2,1,		// e
		//0,3,2,1,  		// l
		//0,3,2,1,  		// l
		//3,3,2,1			// o
	//};	

	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

	bool success = true;
	//int expectedIndex = 0;
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

	BOOST_CHECK(success);
}


BOOST_AUTO_TEST_CASE(test3)
{
	std::cout << "Test 3 - BitFileReader read hello file 3 bits at a time until the end" << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

	bool success = true;
	//int expectedIndex = 0;

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
	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test4)
{
	std::cout << "Test 4 - BitFileReader read in memory 3 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitStringReader r(std::string(helloStr,helloStr+size));

	bool success = true;
	//int expectedIndex = 0;

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
	BOOST_CHECK(success);
}


BOOST_AUTO_TEST_CASE(test5)
{
	std::cout << "Test 5 - BitFileReader read in hello text file 8 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

	bool success = true;
	//int expectedIndex = 0;

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
	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test6)
{
	std::cout << "Test 6 - BitFileReader read in hello text file 12 bits at a time. " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

	bool success = true;
	//int expectedIndex = 0;

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
	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test7)
{
	std::cout << "Test 7 - BitFileReader read in hello text file 12 bits until EOF " << std::endl;

	const char* helloStr = "hello";
	int size = strlen(helloStr);

	BitFileReader r("../testdata/hello.txt");

	bool success = true;
	//int expectedIndex = 0;

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
	BOOST_CHECK(success);
}


BOOST_AUTO_TEST_CASE(test8)
{
	std::cout << "Test 8 - BitFileWriter write hello 1 bit at a time into a text file, then using BitFileReader to read it back." << std::endl;

	bool success = true;

	const char* helloStr = "hello";
	int size = strlen(helloStr);
	int totalBits = 8 * size;

	{
		BitFileWriter w("../testdata/hello.out");

		for( int i = 0; i < totalBits; ++i)
		{
			int bitIndex = (i) % 8;
			int byteIndex = (i) / 8;
			u32 bit = (helloStr[byteIndex] >> bitIndex) & 0x1;		
			w.writeBits(bit,1);
		}
	}

	{
		BitFileReader r("../testdata/hello.out");

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
	BOOST_CHECK(success);
}

bool nBitsInAndOutHelloTest(int bitWordSize)
{
	const char* helloStr = "hello";
	int size = strlen(helloStr);
	int totalBits = 8 * size;
	bool success = true;

	{
		BitFileWriter w("../testdata/hello.out");

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
		BitFileReader r1("../testdata/hello.out");
		BitFileReader r2("../testdata/hello.txt");

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


BOOST_AUTO_TEST_CASE(test9)
{
	std::cout << "Test 9 - BitFileWriter write hello 2 bits at a time into a text file, then using BitFileReader to read it back." << std::endl;

	// Pass.
	BOOST_CHECK( nBitsInAndOutHelloTest(2));
}
BOOST_AUTO_TEST_CASE(test10)
{
	std::cout << "Test 10 - BitFileWriter write hello 3 bits at a time into a text file, then using BitFileReader to read it back." << std::endl;

	// Pass.
	BOOST_CHECK( nBitsInAndOutHelloTest(3));
}
BOOST_AUTO_TEST_CASE(test11)
{
	std::cout << "Test 11 - BitFileWriter write hello 12 bits at a time into a text file, then using BitFileReader to read it back." << std::endl;

	// Pass.
	BOOST_CHECK( nBitsInAndOutHelloTest(12));
}

BOOST_AUTO_TEST_CASE(test12)
{
	std::cout << "Test 12 - BitFileWriter write hello 8 bits at a time into a text file, then using BitFileReader to read it back." << std::endl;

	// Pass.
	BOOST_CHECK( nBitsInAndOutHelloTest(8));
};

BOOST_AUTO_TEST_CASE(test13)
{
	std::cout << "Test 13 - BitFileWriter write 1 666 times into a file, then use BitFileReader to read it back." << std::endl;
	
	bool success = true;
	
	int bitCount = 666;

	{
		BitFileWriter w("../testdata/ones_666.dat");
		for( int i = 0; i < bitCount; ++i )
			w.writeBits(0x1,1);
	}
	{
		BitFileReader r("../testdata/ones_666.dat");
		for( int i = 0; i < bitCount; ++i)
		{
			u32 word = 0;
			int bitsRead = r.readBits(&word,1);
			
			if(bitsRead != 1)
			{
				std::cout << "\tExpected to read 1 bit but read " << bitsRead << " at index " << i << std::endl;
		
			}

			if( word != 1 )
			{
				std::cout << 
					"\t" <<
					"Failed at bits: " << i << std::endl;

				success = false;
			}

		}
	}
	BOOST_CHECK(success);
}


BOOST_AUTO_TEST_CASE(test14)
{
	std::cout << "Test 14 - BitFileWriter write 1 666 times into memory, then use BitFileReader to read it back." << std::endl;

	bool success = true;

	int bitCount = 666;
	int memSize = 700;
	
	std::shared_ptr<u8> mem(new u8[memSize]);
	BitStringWriter w(std::string(mem.get(),mem.get()+memSize));
	for( int i = 0; i < bitCount; ++i )
		w.writeBits(0x1,1);

	w.flush();

	BitStringReader r(w.getString() );

	for( int i = 0; i < bitCount; ++i)
	{
		u32 word = 0;
		r.readBits(&word,1);

		if( word != 1 )
		{
			std::cout << 
				"\t" <<
				"Failed at bits: " << i << 
				" Read " << word <<
				std::endl;

			success = false;
		}

	}

	BOOST_CHECK(success);
}

#define ADJUST(n) (n-256+LzwFast::FirstCode) 

BOOST_AUTO_TEST_CASE(test15)
{
	std::cout << "Test 15 - Compress LZW example strings from Mark Nelson" << std::endl;
	
	bool success = true;

	const char* text_str = "/WED/WE/WEE/WEB/WET";
	const std::string text(text_str,strlen(text_str));
	
	u32 expectedOut[] = { '/','W','E','D',ADJUST(256),'E',ADJUST(260),ADJUST(261),ADJUST(257),'B',ADJUST(260),'T', LzwFast::EndOfData };
	//size_t expectedSize = sizeof(expectedOut) / sizeof(u32);
	
	{
		Codec* codec = new LzwFast();
		BitStringReader r(text);
		BitFileWriter w("../testdata/LzwFast.out");
		codec->encode(&r,&w);
		delete codec;
	}

	BitFileReader r("../testdata/LzwFast.out");
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
				" but read "  << word << 
				" at index " << expectedIndex << std::endl;	
			success = false;
		}
		if( expectedOut[expectedIndex] == LzwFast::EndOfData )
			break;
		expectedIndex++;
	}


	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test16)
{
	std::cout << "Test 16 - BitFileReader Test that we check the correct number of bits read" << std::endl;

	BitFileReader r("../testdata/hello.txt");
	
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
	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test17)
{
	std::cout << "Test 17 - Decompress LZW example strings from Mark Nelson" << std::endl;
	
	bool success = true;

	const std::string text("/WED/WE/WEE/WEB/WET");
	//u32 expectedOut[] = { '/','W','E','D',256,'E',260,261,257,'B',260,'T' };
	//size_t expectedSize = sizeof(expectedOut) / sizeof(u32);
	
	{
		Codec* codec = new LzwFast();
		BitStringReader r(text);
		BitFileWriter w("../testdata/LzwFast.out");
		codec->encode(&r,&w);
		delete codec;
	}


	{
		Codec* codec = new LzwFast();
		BitFileReader r("../testdata/LzwFast.out");
		BitFileWriter w("../testdata/LzwFast.txt");
		codec->decode(&r,&w);
		delete codec;
	}
	{
		BitFileReader r("../testdata/LzwFast.txt");
		char c;
		int i = 0;
		while(r.readBits(&c,8))
		{
			if(c != text[i])
				std::cout << "Expecting " << text[i] << ", but read " << c << std::endl;
			i++;
		}	
	}

	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test18)
{
	std::cout << "Test 18 - Compress Alice in Wonderland and Read it back" << std::endl;

	int success = true;	
	std::unique_ptr<Codec> codec(new LzwFast());
	{
		BitFileReader r("../testdata/alice.txt");
		BitFileWriter w("../testdata/alice.out");
		codec->encode(&r,&w);
	}

	{
		BitFileReader r("../testdata/alice.out");
		BitFileWriter w("../testdata/alice.txtout");
		codec->decode(&r,&w);
	}
	{
		BitFileReader r1("../testdata/alice.txt");
		BitFileReader r2("../testdata/alice.txtout");
		char c1,c2;
		int i = 0;
		while(r1.readBits(&c1,8) && r2.readBits(&c2,8))
		{
			if(c1 != c2)
			{
				std::cout << "Expecting " << c1 << ", but read " << c2 << "@ index " << i << std::endl;
				success=false;
			}
			i++;
		}	
	}

	BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(test19)
{
	std::cout << "Test 19 - Write null terminated string to a file." << std::endl;
	std::string teststr = "testing12345";
	bool success = true;

	{
		// write some null terminated strings.
		BitFileWriter w("../testdata/nullstr.txt");
		success = w.write(teststr);
		w.close();
	}

	BitFileReader r("../testdata/nullstr.txt");
	std::string readstr;
	success &= r.read(&readstr);

	//M_DEBUGTRACE(" - read: " << readstr << " expecting: " << teststr);
	BOOST_CHECK(success && readstr == teststr);
}

BOOST_AUTO_TEST_CASE(test20)
{
	std::cout << "Test 20 - Write null terminated string to memory." << std::endl;
	std::string teststr = "testing12345";
	bool success = true;

	// write some null terminated strings.
	BitStringWriter w("");
	success = w.write(teststr);

	BitStringReader r(w.getString());
	std::string readstr;
	success &= r.read(&readstr);

	//M_DEBUGTRACE(" - read: " << readstr << " expecting: " << teststr);
	BOOST_CHECK(success && readstr == teststr);
}

BOOST_AUTO_TEST_CASE(test21)
{
	std::cout << "Test 21 - Create an empty archive, write it and read it back" << std::endl;

	static const char* kFile = "../testdata/string_archive.bin";
	// write.
	{
		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(kFile));
		auto a = Archive::create(w);
	}

	// read.
	auto r = std::shared_ptr<BitReader>(new BitFileReader(kFile));
	auto a = Archive::load(r);

	// check?
	BOOST_CHECK( a->contents().empty());
}

BOOST_AUTO_TEST_CASE(test22)
{
	static const char* kFile = "../testdata/string_archive.bin";
	static const std::string recordName = "mystring.txt";
	static const std::string testingStr = "testing123...";

	std::cout << "Test 22 - Add a string to an archive and read it back." << std::endl;

	// write.
	{
		auto r = std::shared_ptr<BitReader>(new BitStringReader(testingStr));
		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(kFile));
		auto a = Archive::create(w);
		a->add(recordName, r.get());
	}

	// read.
	auto r = std::shared_ptr<BitReader>(new BitFileReader(kFile));
	auto a = Archive::load(r);

	// check the file is found in the archive.
	auto contents = a->contents();
	auto i = std::find_if(contents.begin(),contents.end(), [=](const Archive::Info& i) { return i.filename == recordName; });
	if(i== contents.end())
	{
		std::cout << "failed to find " << recordName << " in archive" << std::endl;
		for(auto c:contents)
			std::cout << c.filename << std::endl;
		BOOST_CHECK(false);		
	}

	// decode it.
	auto w = std::shared_ptr<BitWriter>(new BitStringWriter(""));
	if(!a->get(recordName,w))
	{
		std::cout << "get failed to get the file" << std::endl;
		BOOST_CHECK(false);		
	}

	// finally check it decoded correctly.
	std::string str = static_cast<BitStringWriter*>(w.get())->getString();
	bool pass = str == testingStr;
	if(!pass)
	{
		std::cout << "strings not the same: " << str << " vs " << testingStr << std::endl;
	}
	
	BOOST_CHECK(pass);
}


BOOST_AUTO_TEST_CASE(test23)
{
	static const char* kFile = "../testdata/alice_archive.bin";
	static const std::string recordName = "alicec.txt";
	static const std::string testFile = "../testdata/alice.txt";

	std::cout << "Test 23 - Add alice in wonderland to an archive and read it back." << std::endl;

	// write.
	{
		auto r = std::shared_ptr<BitReader>(new BitFileReader(testFile.c_str()));
		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(kFile));
		auto a = Archive::create(w);
		a->add(recordName, r.get());
	}

	// read.
	auto r = std::shared_ptr<BitReader>(new BitFileReader(kFile));
	auto a = Archive::load(r);

	// check the file is found in the archive.
	auto contents = a->contents();
	auto i = std::find_if(contents.begin(),contents.end(), [=] (const Archive::Info& i) { return recordName == i.filename; });
	if(i== contents.end())
	{
		std::cout << "failed to find " << recordName << " in archive" << std::endl;
		for(auto c:contents)
			std::cout << c.filename << std::endl;
		BOOST_CHECK(false);		
	}

	// decode it.
	auto w = std::shared_ptr<BitWriter>(new BitStringWriter(""));
	if(!a->get(recordName,w))
	{
		std::cout << "get failed to get the file" << std::endl;
		BOOST_CHECK(false);		
	}

	// finally check it decoded correctly.
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test24)
{
	static const char* kFile = "../testdata/multi_archive.bin";
	static const std::string recordName1 = "alice.txt";
	static const std::string recordName2 = "pan.txt";
	static const std::string dataPath = "../testdata/";
	static const std::string testFile1 = dataPath + recordName1;
	static const std::string testFile2 = dataPath + recordName2;

	std::cout << "Test 24 - Add two files to the archive" << std::endl;

	// Make an archive.
	{
		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(kFile));
		auto a = Archive::create(w);
	
		// write 1.	
		{
			auto r = std::shared_ptr<BitReader>(new BitFileReader(testFile1.c_str()));
			a->add(recordName1, r.get());
		}

		// write 2.
		{
			auto r = std::shared_ptr<BitReader>(new BitFileReader(testFile2.c_str()));
			a->add(recordName2, r.get());
		}
	}

	// read.
	auto r = std::shared_ptr<BitReader>(new BitFileReader(kFile));
	auto a = Archive::load(r);

	// check both files are found in the archive.
	auto contents = a->contents();
	auto i1 = std::find_if(contents.begin(),contents.end(), [=] (const Archive::Info& i) { return i.filename == recordName1; } );;
	auto i2 = std::find_if(contents.begin(),contents.end(), [=] (const Archive::Info& i) { return i.filename == recordName2; } );;

	if(i1== contents.end())
	{
		std::cout << "failed to find " << recordName1 << " in archive" << std::endl;
		for(auto c:contents)
			std::cout << c.filename << std::endl;
		BOOST_CHECK(false);		
	}
	if(i2== contents.end())
	{
		std::cout << "failed to find " << recordName2 << " in archive" << std::endl;
		for(auto c:contents)
			std::cout << c.filename << std::endl;
		BOOST_CHECK(false);		
	}

	// decode them both.
	auto w1 = std::shared_ptr<BitWriter>(new BitFileWriter("../testdata/r1.txt"));
	if(!a->get(recordName1,w1))
	{
		std::cout << "get failed to get file1" << std::endl;
		BOOST_CHECK(false);		
	}
	
	auto w2 = std::shared_ptr<BitWriter>(new BitFileWriter("../testdata/r2.txt"));
	if(!a->get(recordName2,w2))
	{
		std::cout << "get failed to get file2" << std::endl;
		BOOST_CHECK(false);		
	}

	// finally check it decoded correctly.
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test25)
{
	std::cout << "Test 25 - Seek and Tell test" << std::endl;
	static const std::string testFile = "../testdata/seek_test.dat";
	std::streamoff off;
	static const u32 magic = 0xdeadbeaf;

	{
		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(testFile.c_str()));

		for(auto i = 0;i < 789;i++)
			w->writeBits(0, 1);

		w->writeBits(1,1);

		w->seek(34);

		w->writeBits(1,1);
		w->writeBits(0,1);
		w->write<u32>(magic);
	
		off = w->tell();
		off =+ 9;
		w->seek(off);
		
		w->writeBits(1,1);
		w->writeBits(0,1);
		w->write<u32>(magic);
	}
	{
		auto r = std::shared_ptr<BitReader>(new BitFileReader(testFile.c_str()));

		u32 word;
		for(auto i = 0;i < 789;i++)
			r->readBits(&word, 1);

		if(!r->readBits(&word,1) || word != 1)
			BOOST_CHECK(false);

		r->seek(34);
		if(r->readBits(&word,1)!=1 || word != 1)
			BOOST_CHECK(false);
		if(r->readBits(&word,1)!=1 || word != 0)
			BOOST_CHECK(false);
		if(!r->read<u32>(&word) || word != magic)
		{
			std::cout << "read " << std::hex << word << std::dec << std::endl;
			BOOST_CHECK(false);
		}

		off = r->tell();
		off =+ 9;
		r->seek(off);
		
		if(r->readBits(&word,1)!=1 || word != 1)
			BOOST_CHECK(false);
		if(r->readBits(&word,1)!=1 || word != 0)
			BOOST_CHECK(false);
		if(!r->read(&word) || word != magic)
		{
			BOOST_CHECK(false);
		}
	}

	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test26)
{
	std::cout << "Test 26 - Reader File size test" << std::endl;
	static const std::string testFile = "../testdata/alice.txt";

	auto r = std::shared_ptr<BitReader>(new BitFileReader(testFile.c_str()));
	u8 byte;
	std::streamoff b = r->tell();

	r->readBits(&byte,3);
	while(r->read(&byte));

	std::streamoff e = r->tell();

	std::streamoff size = e-b;
	bool pass = size == 167517;	

	if(!pass)
		std::cout << "failed, expecting " << 167517 << " read " << size << std::endl;

	BOOST_CHECK(pass);
}

BOOST_AUTO_TEST_CASE(test27)
{
	std::cout << "Test 27 - Writer File size test" << std::endl;
	static const char* kFile = "../testdata/size_writer.bin";
	static const std::string recordName = "mystring.txt";
	static const std::string testingStr = "testing123...";

	// write.
	auto r = std::shared_ptr<BitReader>(new BitStringReader(testingStr));
	auto w = std::shared_ptr<BitWriter>(new BitFileWriter(kFile));

	w->writeBits(1,3);

	std::streamoff b = w->tell();
	w->write(testingStr);
	std::streamoff e = w->tell();

	std::streamoff size = (e-b)-1;
	bool pass = static_cast<std::streamoff>(testingStr.size()) == size;
	if(!pass)
		std::cout << "string is " << testingStr.size() << ", calculated=" << size << std::endl;
	
	BOOST_CHECK(pass);  // JUST TESTING!
}

BOOST_AUTO_TEST_SUITE_END()


