#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

int VIP_SEATS = 0;
int REGULAR_SEATS = 0;

int *vip_seats;     // 0 occupied, 1 unoccupied
int *regular_seats; // 0 occupied, 1 unoccupied

sem_t vip_semaphore;
sem_t regular_semaphore;
pthread_mutex_t booking_mutex;

struct Customer
{
    int id;
    bool is_vip;
};

void initialize_seats(int vip_seat_count, int regular_seat_count)
{
    vip_seats = new int[vip_seat_count];
    regular_seats = new int[regular_seat_count];
    for (int i = 0; i < vip_seat_count; i++)
    {
        vip_seats[i] = 0;
    }
    for (int i = 0; i < regular_seat_count; i++)
    {
        regular_seats[i] = 0;
    }
}

void *book_seat(void *arg)
{
    Customer *customer = (Customer *)arg;

    if (customer->is_vip) // if vip
    {
        if (sem_trywait(&vip_semaphore) == 0) // if any vip seat is available (wait)
        {
            pthread_mutex_lock(&booking_mutex); // mutex(wait) critical section
            for (int i = 0; i < VIP_SEATS; i++)
            {
                if (vip_seats[i] == 0)
                {
                    // first unoccupied seat
                    vip_seats[i] = customer->id;
                    cout << "VIP Customer " << customer->id << " booked VIP seat " << (i + 1) << endl;
                    pthread_mutex_unlock(&booking_mutex); // mutex(signal)
                    return NULL;
                }
            }
            pthread_mutex_unlock(&booking_mutex); // mutex(signal)
        }
    }

    // reg customer or no vip available
    if (sem_trywait(&regular_semaphore) == 0) // if any regular seat is available (wait)
    {
        pthread_mutex_lock(&booking_mutex); // mutex(wait) critical section
        for (int i = 0; i < REGULAR_SEATS; i++)
        {
            if (regular_seats[i] == 0) // Find the first unoccupied Regular seat
            {
                regular_seats[i] = customer->id;
                cout << (customer->is_vip ? "VIP" : "Regular") << " Customer " << customer->id
                     << " booked Regular seat " << (i + 1) << endl;
                pthread_mutex_unlock(&booking_mutex); // mutex(signal)
                return NULL;
            }
        }
        pthread_mutex_unlock(&booking_mutex); // mutex(signal)
    }

    // all seats booked
    cout << (customer->is_vip ? "VIP" : "Regular") << " Customer " << customer->id << " could not book a seat." << endl;
    return NULL;
}

int main()
{
    int vip_user_count;
    int regular_user_count;

    cout << "Enter the number of VIP seats: ";
    cin >> ::VIP_SEATS;
    while (::VIP_SEATS < 1)
    {
        cout << "Number of VIP seats must be at least 1. Enter again: ";
        cin >> ::VIP_SEATS;
    }
    cout << "Enter the number of regular seats: ";
    cin >> ::REGULAR_SEATS;
    while (::REGULAR_SEATS < 1)
    {
        cout << "Number of regular seats must be at least 1. Enter again: ";
        cin >> ::REGULAR_SEATS;
    }

    cout << "Enter the number of VIP users: ";
    cin >> vip_user_count;
    while (vip_user_count < 1)
    {
        cout << "Number of VIP users must be at least 1. Enter again: ";
        cin >> vip_user_count;
    }
    cout << "Enter the number of regular users: ";
    cin >> regular_user_count;
    while (regular_user_count < 1)
    {
        cout << "Number of regular users must be at least 1. Enter again: ";
        cin >> regular_user_count;
    }

    int total_seats = VIP_SEATS + REGULAR_SEATS;
    int total_users = vip_user_count + regular_user_count;

    // counting semaphore init
    sem_init(&vip_semaphore, 0, VIP_SEATS);
    sem_init(&regular_semaphore, 0, REGULAR_SEATS);

    // mutex init
    pthread_mutex_init(&booking_mutex, NULL);

    initialize_seats(VIP_SEATS, REGULAR_SEATS);

    // Each customer is a thread
    pthread_t *customers = new pthread_t[total_users];
    Customer *customer_data = new Customer[total_users];

    for (int i = 0; i < vip_user_count; i++)
    {
        customer_data[i] = {i + 1, true};
    }

    for (int i = 0; i < regular_user_count; i++)
    {
        customer_data[vip_user_count + i] = {vip_user_count + i + 1, false};
    }

    for (int i = 0; i < total_users; i++)
    {
        pthread_create(&customers[i], NULL, book_seat, &customer_data[i]);
        sleep(1); // 1 sec
    }

    for (int i = 0; i < total_users; i++)
    {
        pthread_join(customers[i], NULL); // wait for all threads
    }

    // memory cleanup
    sem_destroy(&vip_semaphore);
    sem_destroy(&regular_semaphore);
    pthread_mutex_destroy(&booking_mutex);
    delete[] vip_seats;
    delete[] regular_seats;
    delete[] customers;
    delete[] customer_data;

    return 0;
}
