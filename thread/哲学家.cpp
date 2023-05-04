#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_mutex_t chopsticks[5];
sem_t allow;

void stick_init(pthread_mutex_t* chopsticks)
{
    for (int i=0;i<5;i++)
        pthread_mutex_init(&chopsticks[i],NULL);
}


class philosopher{
private:
    int num;
public:
    void eat(void){
        sleep(100);
    }
    void getStick(void){
        
    }
};


int main(void)
{
    
}
