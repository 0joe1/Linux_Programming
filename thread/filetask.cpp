#include "thread_pool.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

struct SearchConfig {
    std::string root_path;    // 要搜索的根目录
    std::string file_type;    // 要搜索的文件类型，如 ".txt"、".cpp" 等
    int max_concurrency;      // 最大并发数
    int max_depth;            // 最大搜索深度
    bool skip_hidden;         // 是否跳过隐藏文件或目录
    std::vector<std::string> skip_paths;   // 要跳过的目录或文件的路径
};

vector<string> collector;

class getfiles : public TASK{
    private:
        thread_pool& pool;
        unsigned int depth;
        struct SearchConfig config;
        string folder;
    private:
        bool isSkip(string folder,struct dirent* dp)
        {
            string skip = folder+dp->d_name;
            bool ret = 0;
            if (strcmp(dp->d_name,".")==0 || strcmp(dp->d_name,"..") ==0)
                return 1;
            if (!config.skip_hidden)
                for (auto it=config.skip_paths.begin();it!=config.skip_paths.end();++it)
                    if (*it == skip)
                        return 1;
            return 0;
        }
        bool inSearch(string file,string file_type)
        {
            return (file.find(file_type) == string::npos)?0:1;
        }
    public:
        getfiles(unsigned int d,struct SearchConfig c,string f,thread_pool& p)
            :depth(d),config(c),folder(f),pool(p) {}
        void dosome()
        {
            if (depth >= config.max_depth){
                return;
            }
            cout <<depth<<endl;
            DIR *dirp;
            struct dirent *dp;
            struct stat sb;

            const char* dirpath = folder.c_str();
            dirp = opendir(dirpath);
            if (dirp == NULL){
                perror("opendir");
                exit(EXIT_FAILURE);
            }

            dp = readdir(dirp);
            while (dp)
            {
                string filename = folder+'/'+dp->d_name;
                if (isSkip(folder,dp)){
                    dp = readdir(dirp);//here
                    continue;
                }
                cout << filename<< depth <<endl;

                lstat(filename.c_str(),&sb);
                if (S_ISDIR(sb.st_mode))
                {
                    unique_ptr<getfiles> newtask=make_unique<getfiles>(depth+1,config,filename,pool);
                    pool.add_task(std::move(newtask));
                }
                if (inSearch(filename,config.file_type))
                {
                    collector.push_back(filename);
                }
                dp = readdir(dirp);
                //cout << dp->d_name<<endl;
            }
        }
};

int main(void)
{
    vector<string> filepaths;
    struct SearchConfig config;
    config.skip_hidden = true;
    config.file_type = ".txt";
    config.max_depth = 4;
    thread_pool pool = thread_pool(10000,3);
    getfiles task(0, config, "/home/username/Desktop",pool);
    task.dosome();
    //sleep(2);
    pool.wait_done();


    return 0;
}
