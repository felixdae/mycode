#ifndef LIBPROCESSUTIL_H
#define LIBPROCESSUTIL_H 
namespace comm
{
    namespace library
    {
        class ProcessUtil
        {
            public:
                static void Daemonize();
                static int32_t Fork();
                static bool should_stop_;
            private:
                ProcessUtil();
        };
    }
}
#endif /* LIBPROCESSUTIL_H */
