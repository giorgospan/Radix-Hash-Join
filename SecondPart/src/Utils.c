#include <stdio.h>/* fprintf() */
#include <stdlib.h>/* malloc(),exit() */
#include <string.h>/* strerror() */
#include <errno.h>/* errno */

#include "Utils.h"

void *allocate(unsigned size,const char *errmsg)
{
	void *ptr;
	if( (ptr = malloc(size)) == NULL )
	{
		fprintf(stderr,"malloc failed[%s]: %s\nExiting...\n\n",errmsg,strerror(errno));
		exit(EXIT_FAILURE);
	}
	return ptr;		
}