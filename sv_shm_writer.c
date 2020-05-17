#include "sv_shm.h"

#include <stdio.h>
#include <unistd.h> // STDIN_FILENO
#include <sys/shm.h>

#include "binary_sems.h"


int main(int argc, char *argv[])
{
    int semid, shmid, bytes, block_count;
    struct shmseg *shmp;
    union semun dummy;

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1)
        ERR_EXIT("semget");

    if (init_sema_available(semid, WRITE_SEM) == -1)
        ERR_EXIT("init_sema_available");
    
    if (init_sema_inuse(semid, READ_SEM) == -1)
        ERR_EXIT("init_sema_inuse");

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1)
        ERR_EXIT("shmget");

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1)
        ERR_EXIT("shmat");

    /* Transfer blocks of data from stdin to shared memory */
    for (block_count = 0, bytes = 0; ; block_count++, bytes += shmp->cnt) {
        if (reserve_sema(semid, WRITE_SEM) == -1) /* Wait for our turn */
            ERR_EXIT("reserve_sema");

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1)
            ERR_EXIT("read");

        if (release_sema(semid, READ_SEM) == -1) /* Give reader a turn */
            ERR_EXIT("release_sema");

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */
        if (shmp->cnt == 0)
            break;
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

    if (reserve_sema(semid, WRITE_SEM) == -1)
        ERR_EXIT("reserve_sema");

    if (semctl(semid, 0, IPC_RMID, dummy) == -1)
        ERR_EXIT("semctl");

    if (shmdt(shmp) == -1)
        ERR_EXIT("shmdt");
    
    if (shmctl(shmid, IPC_RMID, 0) == -1)
        ERR_EXIT("shmctl");

    printf("Sent %d bytes (%d block_count)\n", bytes, block_count);
    exit(EXIT_SUCCESS);
}
