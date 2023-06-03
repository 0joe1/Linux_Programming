#include <iostream>


class MTSearcher{
private:
    struct SearchConfig config;
    thread_pool pool;
    vector<string> filepaths;
private:
public:
    MTSearcher(SearchConfig con)
        :config(con),pool(10000,con.max_concurrency){}
    void search();
};

int main(void)
{
    return 0;
}
