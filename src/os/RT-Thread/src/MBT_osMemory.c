#include <rtthread.h>


void *m_malloc(int size)
{
	return rt_malloc(size);
}

void m_free(void *p)
{
	rt_free(p);
}

char *m_strdup(char* str)
{
	return rt_strdup(str);
}