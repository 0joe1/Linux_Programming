#include "SPSCQueue.hpp"


template <class T>
class thread_pool{
private:
    SPSCQueue<T> tasks;
    SPSCQueue<T> available;
public:
    explicit thread_pool(size_t totTreads)
        :tasks(totTreads),available(totTreads){}
    bool assign_task(void){
        task=tasks.pop();
    }
    bool work(void);
};

int main(void)
{
    return 0;
}

