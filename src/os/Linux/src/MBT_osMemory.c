#include <stdlib.h>

#include "MBT_osMemory.h"



void *m_malloc(int size)
{
	return malloc(size);
}

void m_free(void *p)
{
	free(p);
}


#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L

#include <string.h>

char *m_strdup(char* str)
{
	return strdup(str);
}

#endif