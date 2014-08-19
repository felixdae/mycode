#include <string>
#include "liboption.h"

#include "init.h"

int32_t LoadSetting(int argc, char** argv, struct setting& rs)
{
    const char* candidate = "h:p:H:P:R:W:u:dvl:n:s:";

    comm::library::CommandLineOption cl(argc, argv, candidate);

    if (cl.HasOption('h'))
    {
        rs.listen_ip = cl.GetValue('h', "");
    }
    if (cl.HasOption('p'))
    {
        rs.listen_port = cl.GetValue('p', 0);
    }
    if (cl.HasOption('H'))
    {
        rs.redis_ip = cl.GetValue('H', "");
    }
    if (cl.HasOption('P'))
    {
        rs.redis_port = cl.GetValue('P', 0);
    }
    if (cl.HasOption('R'))
    {
        rs.read_tube = cl.GetValue('R', "");
    }
    if (cl.HasOption('W'))
    {
        rs.write_tube = cl.GetValue('W', "");
    }
    if (cl.HasOption('u'))
    {
        rs.post_url = cl.GetValue('u', "");
    }

    if (cl.HasOption('d'))
    {
        rs.debug = true;
    }
    if (cl.HasOption('v'))
    {
        rs.verbose = true;
    }
    if (cl.HasOption('l'))
    {
        rs.log_path = cl.GetValue('l', "");
    }
    if (cl.HasOption('u'))
    {
        rs.log_file_num = cl.GetValue('n', 0);
    }
    if (cl.HasOption('s'))
    {
        rs.log_file_size = cl.GetValue('s', 0);
    }

//    if (rs.debug)
//    {
//        CLOG->SetLogDebug();
//    }
    return 0;
}


