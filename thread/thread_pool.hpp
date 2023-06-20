#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#include "SPSCQueue.hpp"

class TASK{
public:
    virtual void dosome(void){};
};

class thread_pool{
private:
    SPSCQueue<TASK> tasks;
    vector<pthread_t> ids;
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
                task->dosome();
            }
        return 0;
        }
    }
public:
    explicit thread_pool(size_t tottasks,size_t totTreads)
        :tasks(tottasks){
            ids.resize(totTreads);
            shutdown=false;
            for (int i=0;i<totTreads;i++)
            {
                pthread_t thread;
                pthread_create(&thread,NULL,&threadfunc,this);
                ids[i]=thread;
            }
        }
    ~thread_pool(){
        shutdown=true;
        tasks.liberate();
        for (int i=0;i<ids.size();i++)
            pthread_join(ids[i],NULL);
    }
    bool add_task(unique_ptr<TASK> task){
        return tasks.Push(std::move(task));
    }
    void wait_done(){
        tasks.wait_blocked(ids.size());
    }
};


#endif

