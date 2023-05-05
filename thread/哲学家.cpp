#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <map>

using namespace std;


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
    string name;
public:
    explicit philosopher(int x)
        :num(x){
            static map<int,string> names{
                {1,"Plato"},
                {2,"Kant"},
                {3,"LinZhouYi"},
                {4,"Lao Tzu"},
                {5,"Hegel"}
            };
            name=names[num];
        }
    virtual bool eat(void){
        cout << "philosopher "<< name <<" eating....." << endl;
        sleep(100);
        pthread_mutex_unlock(&chopsticks[num%5]);
        pthread_mutex_unlock(&chopsticks[(num+1)%5]);

        return 1;
    }
    virtual bool getStick(void){
        pthread_mutex_lock(&chopsticks[num%5]);
        pthread_mutex_lock(&chopsticks[(num+1)%5]);

        return 1;
    }
};

void feast1(philosopher p)
{
    sem_wait(&allow);
    p.getStick();
    p.eat();
}




int main(void)
{
    sem_init(&allow,1,4);
    philosopher philosophers[] = {philosopher(1), philosopher(2), philosopher(3), philosopher(4), philosopher(5)};

    for (int i=0;i<5;i++)
        feast1(philosophers[i]);

    return 0;
}
