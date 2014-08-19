#ifndef INIT_H
#define INIT_H 

#include "singleton.h"
#include "liblog.h"

struct setting
{
    std::string listen_ip;
    uint16_t listen_port;
    std::string redis_ip;
    uint16_t redis_port;
    std::string read_tube;
    std::string write_tube;
    std::string post_url;

    bool debug;
    bool verbose;
    std::string log_path;
    uint32_t log_file_num;
    uint32_t log_file_size;

    setting()
    {
        listen_port = redis_port = log_file_num = log_file_size = 0;
        debug = verbose = false;
    }
};

int32_t LoadSetting(int argc, char** argv, struct setting& rs);

#define CLOG comm::library::Singleton<comm::library::Logger>::GetInstance()

#define LOG_NOTICE(format, args...) CLOG->Log(false, "%s:%d " format, __FILE__, __LINE__, ##args)
#define LOG_DEBUG(format, args...) CLOG->Log(true, "%s:%d " format, __FILE__, __LINE__, ##args)

#define FUNC_GUARD() comm::library::FunctionGuard f(__FUNCTION__, CLOG)
#endif /* INIT_H */
