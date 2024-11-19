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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please provide the file name as cmd args" << endl;
        exit(1);
    }

    string filename = argv[1];

    if (filename.empty())
    {
        cout << "Please provide the file name as cmd args" << endl;
        exit(1);
    }

    if (filename.find(".txt") == string::npos)
    {
        cout << "Please provide a .txt file" << endl;
        exit(1);
    }

    sem_t *mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    sem_t *full = sem_open(FULL, O_CREAT, 0644, 0);
    sem_t *empty = sem_open(EMPTY, O_CREAT, 0644, 1);

    int shmid = shmget(SHARED_MEMORY_ID, 20, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        cout << "Shared memory creation failed" << endl;
        exit(1);
    }

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
    {
        cout << "File open failed" << endl;
        exit(1);
    }

    char buffer[20];
    while (true)
    {
        int size = read(fd, buffer, sizeof(buffer));
        if (size <= 0)
        {
            sem_wait(empty);
            sem_wait(mutex);

            char *shm = (char *)shmat(shmid, NULL, 0);
            memset(shm, 0, 20);
            *shm = '$'; // End of file marker
            shmdt(shm);

            sem_post(mutex);
            sem_post(full);
            break;
        }

        sem_wait(empty);
        sem_wait(mutex);

        char *shm = (char *)shmat(shmid, NULL, 0);
        memset(shm, 0, 20);
        memcpy(shm, buffer, size);
        shmdt(shm);

        cout << "20 bytes written to shared memory" << endl;

        sem_post(mutex);
        sem_post(full);
    }

    close(fd);
    sem_close(mutex);
    sem_close(full);
    sem_close(empty);
    sem_unlink(MUTEX);
    sem_unlink(FULL);
    sem_unlink(EMPTY);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}