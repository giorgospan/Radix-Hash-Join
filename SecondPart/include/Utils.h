#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>/* uint64_t */
#define BUFFERSIZE 512

typedef enum Comparison { Less='<', Greater='>', Equal='=' } Comparison;

void *allocate(unsigned size,const char *errmsg);

int compare(uint64_t key,Comparison cmp,uint64_t constant);

#endif