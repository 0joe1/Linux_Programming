#include <iostream>
#include <unistd.h>
#include "thread_pool.hpp"

void func(void* arg)
{
    int g = *(int*)arg;
    printf("hello\n");
    printf("count down %d\n",g);
}

int main(void)
{
    //TASK task;
    //task.func = func;
    thread_pool pool(10,5);
    for (int i=0;i<10;i++)
    {
        sleep(1);
        unique_ptr<TASK> task = std::make_unique<TASK>();
        task->func = func;
        task->arg = (void*)&i;
        pool.add_task(std::move(task));
    }
    sleep(4);
    return 0;
}
