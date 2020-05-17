
#include "binary_sems.h"
#include <errno.h>

typedef enum { FALSE, TRUE } Boolean;
Boolean use_sem_undo = FALSE;
Boolean retry_on_int = TRUE;

int init_sema_available(int sema_id, int sema_num)
{
    union semun arg;

    arg.val = 1;
    return semctl(sema_id, sema_num, SETVAL, arg);
}

int init_sema_inuse(int sema_id, int sema_num)
{
    union semun arg;

    arg.val = 0;
    return semctl(sema_id, sema_num, SETVAL, arg);
}

int reserve_sema(int sema_id, int sema_num)
{
    struct sembuf sops;

    sops.sem_num = sema_num;
    sops.sem_op = -1;
    sops.sem_flg = use_sem_undo ? SEM_UNDO : 0;

    while (semop(sema_id, &sops, 1) == -1) {
        if (errno != EINTR || !retry_on_int)
            return -1;
    }

    return 0;
}

int  release_sema(int sema_id, int sema_num)
{
    struct sembuf sops;

    sops.sem_num = sema_num;
    sops.sem_op = 1;
    sops.sem_flg = use_sem_undo ? SEM_UNDO : 0;

    return semop(sema_id, &sops, 1);
}
