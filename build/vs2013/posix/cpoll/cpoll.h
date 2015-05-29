/*
  cpoll - copyright 2009 by Dan "Heron" Myers (heronblademastr)
  cpoll is released under the LGPL 3, which can be obtained at this
    URL: http://www.gnu.org/copyleft/lesser.html
  In Linux, cpoll transparently passes through to epoll; there should
    be no performance impact.
  In Windows, cpoll requires that you link with a DLL for the required
    functionality (which is why it's LGPL, not GPL).
  http://sourceforge.net/projects/cpoll/

 Usage:
 In Linux, merely include cpoll.h instead of sys/epoll.h, and use
   CPOLL* instead of EPOLL*.
 In Windows, inlude cpoll.h, and link with cpoll.lib.  Put cpoll.dll
   in your program executable's directory.  You must #define WIN32.
 */

#ifndef CPOLL_H
#define CPOLL_H

#ifdef WIN32

/* At this time, cpoll on Windows only supports socket descriptors,
   not file descriptors, *and not cpoll descriptors*.
   This differs from epoll's behavior of allowing any fd. */

enum CPOLL_EVENTS {
    CPOLLIN      = 0x0001,
    CPOLLOUT     = 0x0002,
    CPOLLRDHUP   = 0x0004,
    CPOLLPRI     = 0x0008,
    CPOLLERR     = 0x0010,
    CPOLLHUP     = 0x0020,
    CPOLLET      = 0x0040,
    CPOLLONESHOT = 0x0080
};

enum CPOLL_OPCODES {
    CPOLL_CTL_ADD,
    CPOLL_CTL_DEL,
    CPOLL_CTL_MOD
};

typedef union cpoll_data {
    void* ptr;
    int fd;
    unsigned int u32;
    unsigned long long u64;
} cpoll_data_t;

struct cpoll_event {
    unsigned int events;
    cpoll_data_t data;
};

/* BUILD_CPOLL_DLL should only be defined to build the cpoll dll.
   For linking a project with cpoll, link with cpoll's .lib and
   #include this file.  DO NOT define BUILD_CPOLL_DLL. */
#define CPOLL_FUNCTION

#else /* not WIN32, assume Linux */

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>

#define CPOLLIN       EPOLLIN
#define CPOLLOUT      EPOLLOUT
#define CPOLLRDHUP    EPOLLRDHUP
#define CPOLLPRI      EPOLLPRI
#define CPOLLERR      EPOLLERR
#define CPOLLHUP      EPOLLHUP
#define CPOLLET       EPOLLET
#define CPOLLONESHOT  EPOLLONESHOT

#define CPOLL_CTL_ADD EPOLL_CTL_ADD
#define CPOLL_CTL_DEL EPOLL_CTL_DEL
#define CPOLL_CTL_MOD EPOLL_CTL_MOD

typedef union epoll_data_t cpoll_data_t;
typedef struct epoll_event cpoll_event;

#endif

/*
Function:    int cpoll_startup()
Description: This function does necessary initialization for the cpoll library.
             While this equates to a noop in Linux, in Windows it does not.
             Applications should call this function exactly once on startup.
             The cpoll library should not be used if startup fails!
Parameters:  None
Returns:     0 if startup was successful, nonzero otherwise
*/

#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION int cpoll_startup();
#ifdef __cplusplus
}
#endif
#else
#define cpoll_startup() 0
#endif

/*
Function:    int cpoll_create(int size)
Description: The size is a hint to the kernel regarding the size of the event backing store.
             cpoll_create returns a file descriptor for use with cpoll.
             This file descriptor must be closed with cpoll_close.
Returns:     When successful, returns a non-negative integer identifying the descriptor.
             When an error occurs, returns -1 and errno is set appropriately.
Errors:
             CINVAL
                 size is not positive.
             CNFILE
                 The system limit on the total number of open files has been reached.
             CNOMEM
                 There was insufficient memory to create the kernel object.
*/

#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION int cpoll_create(int size);
#ifdef __cplusplus
}
#endif
#else
#define cpoll_create epoll_create
#endif

/*
Function:    int cpoll_ctl(int cpfd, int opcode, int fd, struct cpoll_event* event)
Description: Control a cpoll descriptor, cpfd, by requesting that the operation opcode
               be performed on the target file descriptor, fd. The event describes the
               object linked to the file descriptor fd.
             Returns zero on success, or -1 on error.
*/
#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION int cpoll_ctl(int cpfd, int opcode, int fd, struct cpoll_event* event);
#ifdef __cplusplus
}
#endif
#else
#define cpoll_ctl epoll_ctl
#endif

/*
Function:    int cpoll_wait(int cpfd, struct cpoll_event* events, int maxevents, int timeout)
Description: Wait for events on the cpoll file descriptor cpfd for a maximum time of timeout
               milliseconds. The memory area pointed to by events will contain the events that
               will be available for the caller. Up to maxevents are returned. The maxevents
               parameter must be greater than zero. Specifying a timeout of -1 makes cpoll_wait
               wait indefinitely, while specifying a timeout equal to zero makes cpoll_wait
               return immediately even if no events are available.
             Returns the number of file descriptors ready for the requested I/O, zero if
               no file descriptor became ready before the requested timeout, or -1 on error.
*/
#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION int cpoll_wait(int cpfd, struct cpoll_event* events, int maxevents, int timeout);
#ifdef __cplusplus
}
#endif
#else
#define cpoll_wait epoll_wait
#endif

/*
Function:    int cpoll_close(int cpfd)
Description: Takes a cpoll file descriptor to close.
             Returns zero on success, or nonzero on error.
             Failure to close a cpoll file descriptor may result in memory leaks
               and/or loss of data.
             While in Linux cpoll_close() is equivalent to close(), in Windows it is not.
               To maintain portability, cpoll_close must always be used for anything created
               with cpoll_create.
*/
#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION int cpoll_close(int cpfd);
#ifdef __cplusplus
}
#endif
#else
#define cpoll_close close
#endif

/*
Function:    void cpoll_cleanup()
Description: This function does necessary cleanup for the cpoll library.
             While this equates to a noop in Linux, in Windows it does not.
             Applications should call this function exactly once before closing.
*/
#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
CPOLL_FUNCTION void cpoll_cleanup();
#ifdef __cplusplus
}
#endif
#else
#define cpoll_cleanup() do { } while(false)
#endif

#endif /* CPOLL_H */
