#include "bitWiseUtil.h"


uint32_t firstHash(uint32_t num, uint32_t significants)
{
	/* Make the hashvalue in binary */
	/* by shifting bits */
	uint32_t hashValue = num;
	hashValue <<= 32 - significants;
	hashValue >>= 32 - significants;

	/* Another way of calculating the hashvalue: 
		hashValue = num & ((1<<significants)-1); */

	// printf("Hash Value is %u\n", hashValue);
	return hashValue;
}

void printNumInBinary(uint32_t num)
{
	if (num)
	{
		printNumInBinary(num >> 1);
		printf("%d", num & 1);
	}
}

uint32_t sizeOfNumInBinary(uint32_t num)
{
	uint32_t size = 0;
	while(num)
	{
		size++;
		num >>= 1;
	}
	return size;
}