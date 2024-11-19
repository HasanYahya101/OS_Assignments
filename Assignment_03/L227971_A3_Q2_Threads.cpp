#include <iostream>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
using namespace std;

#define MUTEX "/mutex"
#define FULL "/full"
#define EMPTY "/empty"
#define SHARED_MEMORY_ID 1234

void *producer(void *arg)
{
    string filename = (char *)arg;

    sem_t *mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    sem_t *full = sem_open(FULL, O_CREAT, 0644, 0);
    sem_t *empty = sem_open(EMPTY, O_CREAT, 0644, 1);

    int id = shmget(SHARED_MEMORY_ID, 20, IPC_CREAT | 0666);
    if (id == -1)
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
        if (size <= 0) // End of file
        {
            sem_wait(empty);
            sem_wait(mutex);

            char *shared_mem = (char *)shmat(id, NULL, 0);
            memset(shared_mem, 0, 20);
            *shared_mem = '$';
            shmdt(shared_mem);

            sem_post(mutex);
            sem_post(full);
            break;
        }
        sem_wait(empty);
        sem_wait(mutex);
        char *shared_memory = (char *)shmat(id, NULL, 0);
        memset(shared_memory, 0, 20);
        memcpy(shared_memory, buffer, size);
        shmdt(shared_memory);
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
    shmctl(id, IPC_RMID, NULL);

    return nullptr;
}

void *consumer(void *arg)
{
    sem_t *mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    sem_t *full = sem_open(FULL, O_CREAT, 0644, 0);
    sem_t *empty = sem_open(EMPTY, O_CREAT, 0644, 1);

    int shmid = shmget(SHARED_MEMORY_ID, 20, IPC_CREAT | 0666);
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
        if (*data == '$') // End of file
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

    return nullptr;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Enter filename as cmd argument" << endl;
        return 1;
    }

    string filename = argv[1];

    sem_t *mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    sem_t *full = sem_open(FULL, O_CREAT, 0644, 0);
    sem_t *empty = sem_open(EMPTY, O_CREAT, 0644, 20);
    pthread_t producer_thread;
    pthread_t consumer_thread;

    pthread_create(&producer_thread, nullptr, producer, (void *)filename.c_str());
    pthread_create(&consumer_thread, nullptr, consumer, nullptr);

    pthread_join(producer_thread, nullptr);
    pthread_join(consumer_thread, nullptr);

    sem_unlink(FULL);
    sem_unlink(EMPTY);
    sem_unlink(MUTEX);
    shmctl(SHARED_MEMORY_ID, IPC_RMID, NULL);

    return 0;
}
