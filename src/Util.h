#ifndef D_UTIL_H
#define D_UTIL_H

template<typename T>
T max(const T a, const T b) 
{
	return a > b ? a : b;
}

template<typename T>
T min(const T a, const T b) 
{
	return a < b ? a : b;
}



#endif //D_UTIL_H