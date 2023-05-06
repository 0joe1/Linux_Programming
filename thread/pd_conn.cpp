#include "SPSCQueue.hpp"
#include <unistd.h>
#define N 20
void* producer(void* arg)
{
    SPSCQueue<int>* q = static_cast<SPSCQueue<int>*> (arg);
    cout <<"create a producer thread" <<endl;
    for (int i=0;i<N;i++)
    {
        unique_ptr<int> item(new int(i));
        q->Push(std::move(item));
    }
    return 0;
}

void* consumer(void* arg)
{
    SPSCQueue<int>* q = static_cast<SPSCQueue<int>*> (arg);
    cout << "create a consumer thread" <<endl;
    for (int i=0;i<N;i++)
    {
        unique_ptr<int> item=std::move(q->pop());
    }
    return 0;
}

int main(void)
{
    SPSCQueue<int> queue(7);
    pthread_t producer_thread,consumer_thread;
    pthread_create(&producer_thread,NULL,producer,&queue);
    pthread_create(&consumer_thread,NULL,consumer,&queue);

    pthread_join(producer_thread,NULL);
    pthread_join(producer_thread,NULL);

    return 0;
}
