#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <map>

using namespace std;


pthread_mutex_t chopsticks[5];
pthread_mutex_t eating;
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
    pthread_t id;
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
        pthread_mutex_lock(&eating);

        cout << "philosopher "<< name <<" eating....." << endl;
        sleep(1);
        pthread_mutex_unlock(&chopsticks[(num-1)%5]);
        pthread_mutex_unlock(&chopsticks[num%5]);

        pthread_mutex_unlock(&eating);

        return 1;
    }
    virtual bool getStick(void){
        pthread_mutex_lock(&chopsticks[(num-1)%5]);
        pthread_mutex_lock(&chopsticks[num%5]);

        return 1;
    }
};

/*方案一：最多允许4个人进餐*/
void* feast1(void* arg)
{
    philosopher* p=static_cast<philosopher*>(arg);
    sem_wait(&allow);
    p->getStick();
    p->eat();
    sem_post(&allow);

    return 0;
}




int main(void)
{
    sem_init(&allow,1,4);
    philosopher philosophers[] = {philosopher(1), philosopher(2), philosopher(3), philosopher(4), philosopher(5)};

    for (int i=0;i<5;i++)
        pthread_create(&philosophers[i].id,NULL,feast1,&philosophers[i]);

    for (int i=0;i<5;i++)
        pthread_join(philosophers[i].id,NULL);

    sem_destroy(&allow);
    for (int i=0;i<5;i++)
        pthread_mutex_destroy(&chopsticks[i]);
    return 0;
}
