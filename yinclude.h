/**************************************************************
File			:
Description		:
Date Created	:
Created By		:
Last Modified	:
Last Reviewed	:
**************************************************************/

#ifndef _MB_YWEBYOG_H
#define _MB_YWEBYOG_H

#ifdef linux
#define __LINUX__
#endif // linux

#define BACK_LOG            1000        // Maximum length of the queue of pending connections
#define DEFAULT_ALLOC_LEN   64 * 1024   // Default length used for allocation
                                        // This is done to avoid frequent reallocation in yString                                            

#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
//#define YSOCKET_ERROR  SOCKET_ERROR
//#define YENOTCONN      WSAENOTCONN
//
//#define YSHUT_RD		SD_RECEIVE
//#define YSHUT_WR		SD_SEND
//#define YSHUT_RDWR		SD_BOTH
//#define YTHREAD_HANDLE	HANDLE
//#include <windows.h>
//#include <Winsock2.h>
//#include <shlobj.h>
//#include <process.h>
//#include <io.h>
//#include <limits.h>
//#include <direct.h>
//#include <errno.h>
//
//typedef  SOCKADDR YSOCKADDR;
//typedef int socklen_t;

#else

//typedef int SOCKET;
//#define YSOCKET_ERROR  (-1)
//#define YENOTCONN		ENOTCONN
//
//#define YSHUT_RD		SHUT_RD
//#define YSHUT_WR		SHUT_WR
//#define YSHUT_RDWR		SHUT_RDWR
//#define YTHREAD_HANDLE	pthread_t
//
//#ifndef INVALID_SOCKET
//#define INVALID_SOCKET  (SOCKET)(~0)
//#endif //INVALID_SOCKET
//typedef const struct sockaddr YSOCKADDR;
//#include <errno.h>
//#include <syslog.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <pthread.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
//#include <stdarg.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netinet/tcp.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
#include <ctype.h>
#include <locale.h>
//#include <utime.h>

#ifdef __APPLE__
//#include <sys/uio.h>
#else
//#include <sys/io.h>
#endif // __APPLE__

#endif //_WIN32

//#include <stddef.h>
//#include <fcntl.h>

//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>

//#include <time.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <assert.h>
//#include <limits.h>

#include "ystring.h"

#endif //_YWEBYOG_H

