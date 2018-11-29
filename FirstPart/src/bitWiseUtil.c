#include "bitWiseUtil.h"


uint32_t firstHash(uint32_t num, uint32_t significants)
{
	uint32_t hashValue;

	/**
	 * Calculate the hashValue by shifting back n forth
	 */
	// hashValue = num;
	// hashValue <<= 32 - significants;
	// hashValue >>= 32 - significants;

	/**
	 * Much faster way for calculating the hashValue
	 * of a given number 
	 */
	hashValue = num & ((1<<significants)-1); 

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