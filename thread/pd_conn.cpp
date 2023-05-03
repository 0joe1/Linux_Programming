#include <iostream>
#include <utility>
#include <pthread.h>
#include <memory>
#include <queue>


#define N 10

using namespace std;

template<class T>
class SPSCQueue {
private:
    int cap;
    queue<std::unique_ptr<T>> line;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_n = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_f = PTHREAD_COND_INITIALIZER;

public:
    explicit SPSCQueue(size_t capacity)
        :cap(capacity){}

    virtual bool Push(std::unique_ptr<T> item){
        pthread_mutex_lock(&mtx); //加锁

        //若满，则阻塞等待
        while(line.size() == cap)
            pthread_cond_wait(&cond_f,&mtx);

        //生产，若原来没有就提醒消费者可以消费了
        line.push(item);
        if (line.size()==1)
            pthread_cond_signal(&cond_n);

        //别忘记解锁
        pthread_mutex_unlock(&mtx);

        return 1;
    }
    virtual std::unique_ptr<T> pop(){
        pthread_mutex_lock(&mtx);

        //若空，则阻塞等待生产者生产
        while (line.size()==0)
            pthread_cond_wait(&cond_n,&mtx);

        //消费，若原来是满的就提醒生产者又可以生产了
        unique_ptr<T> ret = std::move(line.front());
        line.pop();
        if (line.size()==cap-1)
            pthread_cond_signal(&cond_f);

        pthread_mutex_unlock(&mtx);

        return ret;
    }
    virtual ~SPSCQueue(){
        pthread_mutex_destroy(&mtx);
        pthread_cond_destroy(&cond_n);
        pthread_cond_destroy(&cond_f);
    }
};

void* producer(void* arg)
{
    SPSCQueue<int>* q = static_cast<SPSCQueue<int>*> (arg);
    cout <<"create a producer thread" <<endl;
    for (int i=0;i<N;i++)
    {
        unique_ptr<int> item(new int(i));
        q->Push(std::move(item));
        cout << "produced item " << i << endl;
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
        cout << "consumed item " << *item<<endl;
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
