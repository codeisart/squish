#include "BasicTypes.h"

class Primes
{
public:
	static u64 s_primes[];
	static u64 s_nPrimes;
	
	static u64 findGreaterThan(u64 n);
	static u64 findLessThan(u64 n);
};
