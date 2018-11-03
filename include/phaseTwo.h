#ifndef H_PHASETWO
#define H_PHASETWO

/* Initializes indexArray's entries of 
	the given relation */
void initializeIndexArray(struct relation* R);

/* Deallocates space allocated for index*/
void deleteIndexArray(struct index** ind);


#endif
