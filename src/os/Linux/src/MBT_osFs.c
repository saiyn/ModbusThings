#include "MBT_os.h"
#include "MBT_osFs.h"
#include "zlib.h"

#define COMPRESS_STEP_SIZE 163840

int m_open(const char *pathname, int flags, ...)
{
	
	return open(pathname, flags);
}


ssize_t m_write(int fd, const void *buf, size_t len)
{
  int64_t nleft = len;
  int64_t nwritten = 0;
  char *  tbuf = (char *)buf;

  while (nleft > 0) {
    nwritten = write(fd, (void *)tbuf, (size_t)nleft);
    if (nwritten < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }
    nleft -= nwritten;
    tbuf += nwritten;
  }

  return len;
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



int m_compressFile(char *srcFileName, char *destFileName) {
  int32_t ret = 0;
  int32_t len = 0;
  char *  data = malloc(COMPRESS_STEP_SIZE);
  FILE *  srcFp = NULL;
  gzFile  dstFp = NULL;

  srcFp = fopen(srcFileName, "r");
  if (srcFp == NULL) {
    ret = -1;
    goto cmp_end;
  }

  int32_t fd = open(destFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd < 0) {
    ret = -2;
    goto cmp_end;
  }

  dstFp = gzdopen(fd, "wb6f");
  if (dstFp == NULL) {
    ret = -3;
    close(fd);
    goto cmp_end;
  }

  while (!feof(srcFp)) {
    len = (int32_t)fread(data, 1, COMPRESS_STEP_SIZE, srcFp);
    (void)gzwrite(dstFp, data, len);
  }

cmp_end:
  if (srcFp) {
    fclose(srcFp);
  }
  if (dstFp) {
    gzclose(dstFp);
  }
  free(data);

  return ret;
}


void m_removeOldLogFiles(char *rootDir, int32_t keepDays) {
  DIR *dir = opendir(rootDir);
  if (dir == NULL) return;

  int64_t        sec = time(NULL);
  struct dirent *de = NULL;

  while ((de = readdir(dir)) != NULL) {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

    char filename[1024];
    snprintf(filename, 1023, "%s/%s", rootDir, de->d_name);
    if (de->d_type & DT_DIR) {
      continue;
    } else {
      int32_t len = (int32_t)strlen(filename);
      if (len > 3 && strcmp(filename + len - 3, ".gz") == 0) {
        len -= 3;
      }

      int64_t fileSec = 0;
      for (int i = len - 1; i >= 0; i--) {
        if (filename[i] == '.') {
          fileSec = atoll(filename + i + 1);
          break;
        }
      }

      if (fileSec <= 100) continue;
      int32_t days = (int32_t)(abs(sec - fileSec) / 86400 + 1);
      if (days > keepDays) {
        (void)remove(filename);
        //uInfo("file:%s is removed, days:%d keepDays:%d", filename, days, keepDays);
      } else {
        //uTrace("file:%s won't be removed, days:%d keepDays:%d", filename, days, keepDays);
      }
    }
  }

  closedir(dir);
  rmdir(rootDir);
}