#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

typedef unsigned long long u64;

int main(int argc, char** argv)
{
	static const float percentBigger = 1.25f; // 25%

	std::cout << "Loading primes...";
	std::ifstream in( "primes.txt" );
	std::vector<u64> primesList;
	u64 i;
	while(in >> i)
		primesList.push_back(i);
	
	std::cout << "loaded " << primesList.size() << "." << std::endl;

	std::cout << "calculating tables at " << percentBigger << " bigger than needed, rounded to nearest prime" << std::endl;
	for(int i = 9; i < 32; ++i)
	{
		u64 tableSize = 1 << i;
		tableSize = (int) ceil((float)tableSize * percentBigger);
		
		auto r = std::upper_bound(primesList.begin(), primesList.end(), tableSize );
		if( r != primesList.end())
		{
			std::cout << "CodeSize: " << i << " TableSize: " << *r << std::endl;
		}
	}

	return 0;
}
