#include "SPSCQueue.hpp"

class TASK{
public:
    void dosome(void)
    {
        cout << "a task" << endl;
    }
};

class thread_pool{
private:
    SPSCQueue<TASK> tasks;
private:
    static void* threadfunc(void* arg)
    {
        thread_pool* self = static_cast<thread_pool*> (arg);
        while(1)
        {
            unique_ptr<TASK> task=std::move(self->tasks.pop());
            task->dosome();
        }
        return 0;
    }
public:
    explicit thread_pool(size_t tottasks,size_t totTreads)
        :tasks(tottasks){
            for (int i=0;i<totTreads;i++)
            {
                pthread_t thread;
                pthread_create(&thread,NULL,&threadfunc,this);
            }
        }
    bool add_task(unique_ptr<TASK> task){
        return tasks.Push(std::move(task));
    }
};

int main(void)
{
    thread_pool pool(7,4);
    for (int i=0;i<9;i++)
        pool.add_task(std::make_unique<TASK>());
    return 0;
}

