#include "thread_pool.hpp"
#include <unistd.h>
#include <semaphore.h>

sem_t sem;

class Mytask : public TASK{
public:
    void dosome(void)
    {
        pthread_t thread;
        thread=pthread_self();
        sem_wait(&sem);
        cout << thread <<" doing a task" << endl;
        sem_post(&sem);
    }
};


int main(void)
{
    sem_init(&sem,1,1);
    thread_pool pool(7,3);
    for (int i=0;i<10;i++)
        pool.add_task(std::make_unique<Mytask>());
    sleep(2);

    sem_destroy(&sem);
    return 0;
}

