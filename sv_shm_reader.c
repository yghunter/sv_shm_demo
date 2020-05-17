#include "sv_shm.h"

#include <stdio.h>
#include <unistd.h> // STDOUT_FILENO write
#include <sys/shm.h>
#include "binary_sems.h" 


int main(int argc, char *argv[])
{
    int semid, shmid, block_count, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        ERR_EXIT("semget");

    shmid  = shmget(SHM_KEY, 0, 0);
    if (shmid == -1)
        ERR_EXIT("shmget");

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *) -1)
        ERR_EXIT("shmat");

    /* Transfer blocks of data from shared memory to stdout */
    for (block_count = 0, bytes = 0; ; block_count++) {
        if (reserve_sema(semid, READ_SEM) == -1) /* Wait for our turn */
            ERR_EXIT("reserve_sema");

        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            ERR_EXIT("partial/failed write");

        if (release_sema(semid, WRITE_SEM) == -1) /* Give writer a turn */
            ERR_EXIT("release_sema");
    }

    if (shmdt(shmp) == -1)
        ERR_EXIT("shmdt");

    /* Give writer one more turn, so it can clean up */
    if (release_sema(semid, WRITE_SEM) == -1)
        ERR_EXIT("release_sema");

    printf("Received %d bytes (%d block_count)\n", bytes, block_count);
    exit(EXIT_SUCCESS);
}
