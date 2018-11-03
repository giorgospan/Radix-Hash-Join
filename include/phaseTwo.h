#ifndef H_PHASETWO
#define H_PHASETWO

/* Initializes indexArray's entries of 
	the given relation */
void initializeIndexArray(struct relation* R);

/* Creates an index for every bucket of
	the given relation */
void buildIndexPerBucket(struct relation* R);

/* Hash value for phase two */
/* We might need to experiment with a couple of 
	hash functions in order to find the best one */
uint32_t secondHash(uint32_t num);

/* Deallocates space allocated for index*/
void deleteIndexArray(struct index** ind);


#endif
