#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "libprocessutil.h"

namespace comm
{
    namespace library
    {
        bool ProcessUtil::should_stop_ = true;

        void sig_handle(int signo)
        {
            ProcessUtil::should_stop_ = true;
        }

        int32_t ProcessUtil::Fork()
        {
            return (int32_t)fork();
        }

        void ProcessUtil::Daemonize()
        {
            umask(0);

            struct rlimit rl;
            if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
            {
                perror("getrlimit");
                exit(-1);
            }

            pid_t pid;
            if ((pid = fork()) < 0)
            {
                perror("fork");
                exit(-1);
            }
            else if(pid != 0)
            {
                exit(0);
            }

            setsid();

            signal(SIGHUP, SIG_IGN);
            signal(SIGINT, SIG_IGN);
            signal(SIGQUIT, SIG_IGN);
            signal(SIGPIPE, SIG_IGN);
            signal(SIGURG, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
            signal(SIGTTIN, SIG_IGN);
            signal(SIGTTOU, SIG_IGN);
            signal(SIGALRM, SIG_IGN);
            signal(SIGCHLD, SIG_IGN);
            signal(SIGTERM, sig_handle);

            if ((pid = fork()) < 0)
            {
                perror("fork");
                exit(-1);
            }
            else if(pid != 0)
            {
                exit(0);
            }

//            if (chdir("/") < 0)
//            {
//                perror("chdir");
//                exit(1);
//            }

            if (rl.rlim_max == RLIM_INFINITY)
            {
                rl.rlim_max = 1024;
            }

            for (int i = 0; i < rl.rlim_max; ++i)
            {
                close(i);
            }

            int fd0, fd1, fd2;
            fd0 = open("/dev/null", O_RDWR);
            fd1 = dup(0);
            fd2 = dup(0);

            ProcessUtil::should_stop_ = false;
        }
    }
}
