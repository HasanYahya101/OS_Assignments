#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
using namespace std;

#define MUTEX "/mutex"
#define FULL "/full"
#define EMPTY "/empty"
#define SHARED_MEMORY_ID 1234
#define BUFFER_SIZE 20

int main()
{
    sem_t *mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    sem_t *full = sem_open(FULL, O_CREAT, 0644, 0);
    sem_t *empty = sem_open(EMPTY, O_CREAT, 0644, BUFFER_SIZE);

    int shmid = shmget(SHARED_MEMORY_ID, BUFFER_SIZE, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        cerr << "Shared memory access failed" << endl;
        exit(1);
    }

    while (true)
    {
        sem_wait(full);
        sem_wait(mutex);

        char *data = (char *)shmat(shmid, NULL, 0);
        if (*data == '$')
        {
            shmdt(data);
            sem_post(mutex);
            sem_post(empty);
            break;
        }

        cout << "Received: " << data << endl;
        shmdt(data);

        // wait for user to enter a key
        cout << "Press any key to continue...";
        cin.get();

        sem_post(mutex);
        sem_post(empty);
    }

    sem_close(mutex);
    sem_close(full);
    sem_close(empty);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}