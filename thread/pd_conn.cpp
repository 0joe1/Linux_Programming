#include <iostream>
#include <pthread.h>
#include <memory>
#include <queue>

using namespace std;

//class product{
//public:
//    int data;
//    bool add(int data){
//
//    }
//}


template<class T>
class SPSCQueue {
private:
    int cap;
    queue<std::unique_ptr<T>> line;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_n = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_f = PTHREAD_COND_INITIALIZER;

public:
    explicit SPSCQueue(int capacity)
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
        while (line.size==0)
            pthread_cond_wait(&cond_n,&mtx);

        //消费，若原来是满的就提醒生产者又可以生产了
        unique_ptr<T> ret = line.front();
        line.pop();
        if (line.size()==cap-1)
            pthread_cond_signal(&cond_f);

        pthread_mutex_unlock(&mtx);

        return ret;
    }
    virtual ~SPSCQueue() = 0;
};

