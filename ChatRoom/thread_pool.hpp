#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#include "SPSCQueue.hpp"
#include "myerror.hpp"

using callback=void(*)(void*);

struct TASK{
    callback func;
    void* arg;

    TASK(callback f=nullptr,void* a=nullptr):
        func(f),arg(a){}
};

class thread_pool{
private:
    SPSCQueue<TASK> tasks;
    bool shutdown;
private:
    static void* threadfunc(void* arg)
    {
        thread_pool* self = static_cast<thread_pool*> (arg);
        while(1)
        {
            if (self->shutdown)
                return 0;

            unique_ptr<TASK> task=std::move(self->tasks.pop());

            if (task != NULL){
                task->func(task->arg);
            }
        }
        return 0;
    }
public:
    explicit thread_pool(size_t tottasks,size_t totTreads)
        :tasks(tottasks){
            shutdown=false;
            for (int i=0;i<totTreads;i++)
            {
                pthread_t thread;
                pthread_create(&thread,NULL,&threadfunc,this);
                pthread_detach(thread);
            }
        }
    ~thread_pool(){
        shutdown=true;
        tasks.liberate();
    }
    bool add_task(unique_ptr<TASK> task){
        return tasks.Push(std::move(task));
    }
};


#endif

