#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>/* uint64_t */
#define BUFFERSIZE 512

#define MALLOC_CHECK(M)                                                         \
    if(!M){                                                                     \
        fprintf(stderr,"[ERROR] MALLOC_CHECK: %s : %d\n", __FILE__, __LINE__);  \
        exit(EXIT_FAILURE);                                                     \
    }


typedef enum Comparison { Less='<', Greater='>', Equal='=' } Comparison;

int compare(uint64_t key,Comparison cmp,uint64_t constant);

/* Power functions */
uint64_t power(uint64_t base, uint64_t exponent);
uint64_t linearPower(uint64_t base, uint64_t exponent);

#endif
