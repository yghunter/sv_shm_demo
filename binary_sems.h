
#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H

#include <sys/sem.h>

/* Used in calls to semctl() */
union semun { 
    int                 val;
    struct semid_ds *   buf;
    unsigned short *    array;
#if defined(__linux__)
    struct seminfo *    __buf;
#endif
};

 /* Initialize semaphore to 1 (i.e., "available") */
int init_sema_available(int sema_id, int sema_num);

/* Initialize semaphore to 0 (i.e., "in use") */
int init_sema_inuse(int sema_id, int sema_num);

/* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
   set to EINTR if operation was interrupted by a signal handler */

/* Reserve semaphore - decrement it by 1 */
int reserve_sema(int sema_id, int sema_num);

/* Release semaphore - increment it by 1 */
int release_sema(int sema_id, int sema_num);

#endif
