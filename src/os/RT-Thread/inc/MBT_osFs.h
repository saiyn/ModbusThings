#ifndef _MBT_OS_FS_H_
#define _MBT_OS_FS_H_




int m_open(const char *pathname, int flags, ...);

int m_stat(const char *pathname, struct stat *statbuf);

int m_rename(const char *oldpath, const char *newpath);

int m_write(int fd, const void *buf, size_t len);

int m_read(int fd, void *buf, size_t len);

int m_unlink(const char *pathname);

int m_access(const char *pathname, int mode);

int m_close(int d);







#endif
