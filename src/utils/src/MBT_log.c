#include "MBT_os.h"

#include "MBT_config.h"

#include "MBT_log.h"

#define LOG_FILE_NAME_LEN          300

#define MAX_LOGLINE_SIZE              (1000)
#define MAX_LOGLINE_BUFFER_SIZE       (MAX_LOGLINE_SIZE + 10)
#define MAX_LOGLINE_CONTENT_SIZE      (MAX_LOGLINE_SIZE - 100)
#define MAX_LOGLINE_DUMP_SIZE         (65 * 1024)
#define MAX_LOGLINE_DUMP_BUFFER_SIZE  (MAX_LOGLINE_DUMP_SIZE + 10)
#define MAX_LOGLINE_DUMP_CONTENT_SIZE (MAX_LOGLINE_DUMP_SIZE - 100)

#define LOG_BUF_BUFFER(x) ((x)->buffer)
#define LOG_BUF_START(x)  ((x)->buffStart)
#define LOG_BUF_END(x)    ((x)->buffEnd)
#define LOG_BUF_SIZE(x)   ((x)->buffSize)
#define LOG_BUF_MUTEX(x)  ((x)->buffMutex)


typedef struct {
  char *          buffer;
  int32_t         buffStart;
  int32_t         buffEnd;
  int32_t         buffSize;
  int32_t         minBuffSize;
  int32_t         fd;
  int32_t         stop;
  pthread_t       asyncThread;
  pthread_mutex_t buffMutex;
} SLogBuff;

typedef struct {
  int32_t fileNum;
  int32_t maxLines;
  int32_t lines;
  int32_t flag;
  int32_t openInProgress;
  pid_t   pid;
  char    logName[LOG_FILE_NAME_LEN];
  SLogBuff *      logHandle;
  pthread_mutex_t logMutex;
} SLogObj;


static SLogObj   msLogObj = { .fileNum = 1 };
int32_t writeInterval = DEFAULT_LOG_INTERVAL;
static int8_t   msLogInited = 0;
int32_t msLogKeepDays = 0;


static SLogBuff *logBuffNew(int32_t bufSize) {
  SLogBuff *tLogBuff = NULL;

  tLogBuff = MBT_calloc(1, sizeof(SLogBuff));
  if (tLogBuff == NULL) return NULL;

  tLogBuff->buffer = MBT_malloc(bufSize);
  if (tLogBuff->buffer == NULL) goto _err;

  tLogBuff->buffStart = tLogBuff->buffEnd = 0;
  tLogBuff->buffSize = bufSize;
  tLogBuff->minBuffSize = bufSize / 10;
  tLogBuff->stop = 0;

  if (pthread_mutex_init(&tLogBuff->buffMutex, NULL) < 0) goto _err;

  return tLogBuff;

_err:
  MBT_free(tLogBuff->buffer);
  MBT_free(tLogBuff);
  return NULL;
}


static int32_t openLogFile(char *fn, int32_t maxLines, int32_t maxFileNum) {

  char        name[LOG_FILE_NAME_LEN + 50] = "\0";
  struct stat logstat0, logstat1;
  int32_t     size;

  msLogObj.maxLines = maxLines;
  msLogObj.fileNum = maxFileNum;
  
  strncpy(msLogObj.logName, fn, sizeof(msLogObj.logName));

  if (strlen(fn) < LOG_FILE_NAME_LEN + 50 - 2) {
    strcpy(name, fn);
    strcat(name, ".0");
  }
  bool log0Exist = stat(name, &logstat0) >= 0;

  if (strlen(fn) < LOG_FILE_NAME_LEN + 50 - 2) {
    strcpy(name, fn);
    strcat(name, ".1");
  }
  bool log1Exist = stat(name, &logstat1) >= 0;
  
  // if none of the log files exist, open 0, if both exists, open the old one
  if (!log0Exist && !log1Exist) {
    msLogObj.flag = 0;
  } else if (!log1Exist) {
    msLogObj.flag = 0;
  } else if (!log0Exist) {
    msLogObj.flag = 1;
  } else {
    msLogObj.flag = (logstat0.st_mtime > logstat1.st_mtime) ? 0 : 1;
  }

  char fileName[LOG_FILE_NAME_LEN + 50] = "\0";
  sprintf(fileName, "%s.%d", msLogObj.logName, msLogObj.flag);


  pthread_mutex_init(&msLogObj.logMutex, NULL);

  umask(0);
  msLogObj.logHandle->fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

  if (msLogObj.logHandle->fd < 0) {
    printf("\nfailed to open log file:%s, reason:%s\n", fileName, strerror(errno));
    return -1;
  }


  // only an estimate for number of lines
  struct stat filestat;
  if (fstat(msLogObj.logHandle->fd, &filestat) < 0) {
    printf("\nfailed to fstat log file:%s, reason:%s\n", fileName, strerror(errno));
    return -1;
  }
  size = (int32_t)filestat.st_size;
  msLogObj.lines = size / 60;

  lseek(msLogObj.logHandle->fd, 0, SEEK_END);

  sprintf(name, "==================================================\n");
  m_write(msLogObj.logHandle->fd, name, (uint32_t)strlen(name));
  sprintf(name, "                new log file                      \n");
  m_write(msLogObj.logHandle->fd, name, (uint32_t)strlen(name));
  sprintf(name, "==================================================\n");
  m_write(msLogObj.logHandle->fd, name, (uint32_t)strlen(name));

  return 0;
}


static int32_t getLogRemainSize(SLogBuff *tLogBuff, int32_t start, int32_t end) {
  int32_t rSize = end - start;

  return rSize >= 0 ? rSize : LOG_BUF_SIZE(tLogBuff) + rSize;
}


static void doWriteLog(SLogBuff *tLogBuff) {
  static int32_t lastDuration = 0;
  int32_t remainChecked = 0;
  int32_t start, end, pollSize;
  
  do {
    if (remainChecked == 0) {
      start = tLogBuff->buffStart;
      end = tLogBuff->buffEnd;

      if (start == end) {
        writeInterval = MAX_LOG_INTERVAL;
        return;
      }

      pollSize = getLogRemainSize(tLogBuff, start, end);
      if (pollSize < tLogBuff->minBuffSize) {
        lastDuration += writeInterval;
        if (lastDuration < LOG_MAX_WAIT_MSEC) {
          break;
        }
      }

      lastDuration = 0;
    }

    if (start < end) {
      m_write(tLogBuff->fd, LOG_BUF_BUFFER(tLogBuff) + start, pollSize);
    } else {
        int32_t tsize = LOG_BUF_SIZE(tLogBuff) - start;
        m_write(tLogBuff->fd, LOG_BUF_BUFFER(tLogBuff) + start, tsize);

        m_write(tLogBuff->fd, LOG_BUF_BUFFER(tLogBuff), end);
    }

    
    if (pollSize < tLogBuff->minBuffSize) {

      if (writeInterval < MAX_LOG_INTERVAL) {
        writeInterval += LOG_INTERVAL_STEP;
      }
    } else if (pollSize >  LOG_BUF_SIZE(tLogBuff)/3) {

      writeInterval = MIN_LOG_INTERVAL;
    } else if (pollSize > LOG_BUF_SIZE(tLogBuff)/4) {
      if (writeInterval > MIN_LOG_INTERVAL) {
        writeInterval -= LOG_INTERVAL_STEP;
      }
    }

    LOG_BUF_START(tLogBuff) = (LOG_BUF_START(tLogBuff) + pollSize) % LOG_BUF_SIZE(tLogBuff);

    start = LOG_BUF_START(tLogBuff);
    end = LOG_BUF_END(tLogBuff);

    pollSize = getLogRemainSize(tLogBuff, start, end);
    if (pollSize < tLogBuff->minBuffSize) {
      break;
    }

    writeInterval = MIN_LOG_INTERVAL;

    remainChecked = 1;
  }while (1);
}


static void *asyncOutputLog(void *param) {
  SLogBuff *tLogBuff = (SLogBuff *)param;
  setThreadName("log");
  
  while (1) {
    m_usleep(writeInterval*1000);

    // Polling the buffer
    doWriteLog(tLogBuff);

    if (tLogBuff->stop) break;
  }

  return NULL;
}


static int32_t startLog() {
  pthread_attr_t threadAttr;
  pthread_attr_init(&threadAttr);
  if (pthread_create(&(msLogObj.logHandle->asyncThread), &threadAttr, asyncOutputLog, msLogObj.logHandle) != 0) {
    return -1;
  }
  pthread_attr_destroy(&threadAttr);
  return 0;
}


static void keepOldLog(char *oldName) {
  if (msLogKeepDays == 0) return;

  int64_t fileSec = m_time(NULL);
  char    fileName[LOG_FILE_NAME_LEN + 20];
  snprintf(fileName, LOG_FILE_NAME_LEN + 20, "%s.%" PRId64, msLogObj.logName, fileSec);

  m_rename(oldName, fileName);
  if (msLogKeepDays < 0) {
    char compressFileName[LOG_FILE_NAME_LEN + 20];
    snprintf(compressFileName, LOG_FILE_NAME_LEN + 20, "%s.%" PRId64 ".gz", msLogObj.logName, fileSec);
    if (m_compressFile(fileName, compressFileName) == 0) {
      (void)remove(fileName);
    }
  }

  m_removeOldLogFiles(MBT_LOG_DIR, ABS(msLogKeepDays));
}

static void *threadToOpenNewFile(void *param) {
  char keepName[LOG_FILE_NAME_LEN + 20];
  sprintf(keepName, "%s.%d", msLogObj.logName, msLogObj.flag);

  msLogObj.flag ^= 1;
  msLogObj.lines = 0;
  char name[LOG_FILE_NAME_LEN + 20];
  sprintf(name, "%s.%d", msLogObj.logName, msLogObj.flag);

  umask(0);

  int32_t fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd < 0) {
    msLogObj.openInProgress = 0;
    msLogObj.lines = msLogObj.maxLines - 1000;
    //uError("open new log file fail! fd:%d reason:%s, reuse lastlog", fd, strerror(errno));
    return NULL;
  }

  (void)lseek(fd, 0, SEEK_SET);

  int32_t oldFd = msLogObj.logHandle->fd;
  msLogObj.logHandle->fd = fd;
  msLogObj.lines = 0;
  msLogObj.openInProgress = 0;

  m_close(oldFd);
  

  keepOldLog(keepName);

  return NULL;
}


static int32_t openNewLogFile() {
  pthread_mutex_lock(&msLogObj.logMutex);

  if (msLogObj.lines > msLogObj.maxLines && msLogObj.openInProgress == 0) {
    msLogObj.openInProgress = 1;

    pthread_t      thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&thread, &attr, threadToOpenNewFile, NULL);
    pthread_attr_destroy(&attr);
  }

  pthread_mutex_unlock(&msLogObj.logMutex);

  return 0;
}


static void copyLogBuffer(SLogBuff *tLogBuff, int32_t start, int32_t end, char *msg, int32_t msgLen) {
  if (start > end) {
    memcpy(LOG_BUF_BUFFER(tLogBuff) + end, msg, msgLen);
  } else {
    if (LOG_BUF_SIZE(tLogBuff) - end < msgLen) {
      memcpy(LOG_BUF_BUFFER(tLogBuff) + end, msg, LOG_BUF_SIZE(tLogBuff) - end);
      memcpy(LOG_BUF_BUFFER(tLogBuff), msg + LOG_BUF_SIZE(tLogBuff) - end, msgLen - LOG_BUF_SIZE(tLogBuff) + end);
    } else {
      memcpy(LOG_BUF_BUFFER(tLogBuff) + end, msg, msgLen);
    }
  }
  LOG_BUF_END(tLogBuff) = (LOG_BUF_END(tLogBuff) + msgLen) % LOG_BUF_SIZE(tLogBuff);
}


static int32_t pushLogBuffer(SLogBuff *tLogBuff, char *msg, int32_t msgLen) {
  int32_t start = 0;
  int32_t end = 0;
  int32_t remainSize = 0;
  static int64_t lostLine = 0;
  char tmpBuf[60] = {0};
  int32_t tmpBufLen = 0;

  if (tLogBuff == NULL || tLogBuff->stop) return -1;

  pthread_mutex_lock(&LOG_BUF_MUTEX(tLogBuff));
  start = LOG_BUF_START(tLogBuff);
  end = LOG_BUF_END(tLogBuff);

  remainSize = (start > end) ? (start - end - 1) : (start + LOG_BUF_SIZE(tLogBuff) - end - 1);

  if (lostLine > 0) {
    sprintf(tmpBuf, "...Lost %"PRId64" lines here...\n", lostLine);
    tmpBufLen = (int32_t)strlen(tmpBuf);
  }

  if (remainSize <= msgLen || ((lostLine > 0) && (remainSize <= (msgLen + tmpBufLen)))) {
    lostLine++;
    pthread_mutex_unlock(&LOG_BUF_MUTEX(tLogBuff));
    return -1;
  }

  if (lostLine > 0) {
    copyLogBuffer(tLogBuff, start, end, tmpBuf, tmpBufLen);
    lostLine = 0;
  }

  copyLogBuffer(tLogBuff, LOG_BUF_START(tLogBuff), LOG_BUF_END(tLogBuff), msg, msgLen);

  pthread_mutex_unlock(&LOG_BUF_MUTEX(tLogBuff));


  return 0;
}


int32_t MBT_initLog(char *logName, int numOfLogLines, int maxFiles) {

    int8_t expected = 0;

    if(!atomic_val_compare_exchange_8(&msLogInited, &expected, 1)){
        return 0;
    }


    msLogObj.logHandle = logBuffNew(MBT_DEFAULT_LOG_BUF_SIZE);
    if (msLogObj.logHandle == NULL) return -1;
    if (openLogFile(logName, numOfLogLines, maxFiles) < 0) return -1;
    if (startLog() < 0) return -1;
    return 0;
}



void MBT_printLog(const char *flags, int32_t dflag, const char *format, ...) {

  if (flags == NULL || format == NULL) {
    return;
  }

  va_list        argpointer;
  char           buffer[MAX_LOGLINE_BUFFER_SIZE] = { 0 };
  int32_t        len;
  struct tm      Tm, *ptm;
  struct timeval timeSecs;
  time_t         curTime;

  gettimeofday(&timeSecs, NULL);
  curTime = timeSecs.tv_sec;
  ptm = localtime_r(&curTime, &Tm);

  len = sprintf(buffer, "%02d/%02d %02d:%02d:%02d.%06d %08" PRId64 " ", ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour,
                ptm->tm_min, ptm->tm_sec, (int32_t)timeSecs.tv_usec, MBT_getSelfPthreadId());
  len += sprintf(buffer + len, "%s", flags);

  va_start(argpointer, format);
  int32_t writeLen = vsnprintf(buffer + len, MAX_LOGLINE_CONTENT_SIZE, format, argpointer);
  if (writeLen <= 0) {
    char tmp[MAX_LOGLINE_DUMP_BUFFER_SIZE] = {0};
    writeLen = vsnprintf(tmp, MAX_LOGLINE_DUMP_CONTENT_SIZE, format, argpointer);
    strncpy(buffer + len, tmp, MAX_LOGLINE_CONTENT_SIZE);
    len += MAX_LOGLINE_CONTENT_SIZE;
  } else if (writeLen >= MAX_LOGLINE_CONTENT_SIZE) {
    len += MAX_LOGLINE_CONTENT_SIZE;
  } else {
    len += writeLen;
  }
  va_end(argpointer);

  if (len > MAX_LOGLINE_SIZE) len = MAX_LOGLINE_SIZE;

  buffer[len++] = '\n';
  buffer[len] = 0;

  if ((dflag & DEBUG_FILE) && msLogObj.logHandle && msLogObj.logHandle->fd >= 0) {
    
    pushLogBuffer(msLogObj.logHandle, buffer, len);


    if (msLogObj.maxLines > 0) {
      atomic_add_fetch_32(&msLogObj.lines, 1);

      if ((msLogObj.lines > msLogObj.maxLines) && (msLogObj.openInProgress == 0)) openNewLogFile();
    }
  }

  if (dflag & DEBUG_SCREEN)
      m_write(1, buffer, (uint32_t)len);
  //if (dflag == 255) nInfo(buffer, len);
}


static void taosStopLog() {
  if (msLogObj.logHandle) {
    msLogObj.logHandle->stop = 1;
  }
}



void MBT_closeLog() {

    if(msLogObj.logHandle != NULL){

        taosStopLog();

        if (msLogObj.logHandle && msLogObj.logHandle->asyncThread != 0) {
            pthread_join(msLogObj.logHandle->asyncThread, NULL);

            msLogObj.logHandle->asyncThread = 0;
        }

        int8_t excepted = 1;

        atomic_val_compare_exchange_8(&msLogInited, &excepted, 0);


        m_free(msLogObj.logHandle);

        msLogObj.logHandle = NULL;

    }
}

void MBT_resetLog() {
  char lastName[LOG_FILE_NAME_LEN + 20];
  sprintf(lastName, "%s.%d", msLogObj.logName, msLogObj.flag);

  // force create a new log file
  msLogObj.lines = msLogObj.maxLines + 10;

  openNewLogFile();


  remove(lastName);
}
