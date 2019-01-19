#include <stdint.h>/* uint64_t */

#include "Utils.h"

int compare(uint64_t key,Comparison cmp,uint64_t constant)
{
	switch(cmp)
	{
		case '=':
			return key==constant;
		case '<':
			return key<constant;
		case '>':
			return key>constant;
	}
}

uint64_t power(uint64_t base, uint64_t exponent)
{
	if (exponent == 0)
		return 1;
	else if (exponent % 2 == 0)
	{
		uint64_t temp = power(base, exponent / 2);
		return temp * temp;
	}
	else
		return base * power(base, exponent - 1);
}

uint64_t linearPower(uint64_t base, uint64_t exponent)
{
	uint64_t res = 1;
	for (uint64_t i = 0; i < exponent; ++i)
		res *= base;
	return res;
}
