#ifndef BUILD_H
#define BUILD_H

/* Initializes indexArray's entries of 
	the given relation */
void initializeIndexArray(struct relation* R);

/* Creates an index for every bucket of
	the given relation */
void buildIndexPerBucket(struct relation* R);

/* Find the first zero in chainArray 
	by following the chain and
	store "posToBeStored" in that place */
void traverseChain(uint32_t chainPos,uint32_t* chainArray,uint32_t posToBeStored);

/* Creates hash value for phase two */
/* We might need to experiment with a couple of 
	hash functions in order to find the best one */
uint32_t secondHash(uint32_t num);

/* Deallocates space allocated for index*/
void deleteIndexArray(struct index** ind);


#endif
