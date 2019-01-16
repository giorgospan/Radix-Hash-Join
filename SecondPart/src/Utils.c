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
