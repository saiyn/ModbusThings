#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "MBT_osFs.h"


int m_open(const char *pathname, int flags, ...)
{
	
	return open(pathname, flags);
}


int m_write(int fd, const void *buf, size_t len)
{
	return write(fd, buf, len);
}

int m_read(int fd, void *buf, size_t len)
{
	return read(fd, buf, len);
}

int m_unlink(const char *pathname)
{
	return unlink(pathname);
}

int m_stat(const char *pathname, struct stat *statbuf)
{
	return stat(pathname, statbuf);
}

int m_rename(const char *oldpath, const char *newpath)
{
	return rename(oldpath, newpath);
}


int m_access(const char *pathname, int mode)
{
	return access(pathname, mode);
}

int m_close(int fd)
{
	return close(fd);
}