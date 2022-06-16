#include <rtthread.h>
#include <dfs_posix.h>

#include "MBT_osFs.h"

int m_open(const char *pathname, int flags)
{
	
	return open(pathname, flags);
}


int m_write(int fd, const void *buf, size_t len)
{
	return write(fd, buf, len);
}

int m_read(int fd, void *buf, size_t len);

int m_unlink(const char *pathname);

int m_stat(const char *pathname, struct stat *statbuf);

int m_rename(const char *oldpath, const char *newpath);


int m_close(int fd)
{
	return close(fd);
}