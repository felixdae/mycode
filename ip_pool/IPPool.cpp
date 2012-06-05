#include <cstdio>
#include <iostream>
#include "library2/libopt.h"
#include "CIPPool.h"

using namespace std;
using namespace basic::library2;
using namespace BCDataService::QQApi;

int Query()
{
    char buf[200];
    CIPPool oIPPool;
    if (oIPPool.SearchInit() != 0)
    {
        cerr << oIPPool.GetLastErrorMsg() << endl;
        return -1;
    }
    while (scanf("%s", buf) != EOF)
    {
        CIPPool::IPInfo oIPInfo;
        int32_t dwRetCode = oIPPool.DoSearch(&oIPInfo, buf);
        if (dwRetCode < 0)
            cerr << oIPPool.GetLastErrorMsg() << endl;
        else if (dwRetCode == 0)
            printf("%u\t%u\t%s\t%s\n", oIPInfo.dwStart, oIPInfo.dwEnd, oIPInfo.sProvince, oIPInfo.sCity);
        else
            cerr << "not found: " << buf << endl;
    }
    if (oIPPool.SearchDone() != 0)
    {
        cerr << oIPPool.GetLastErrorMsg() << endl;
        return -1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    string strIpFilePath;
    bool bDelShm = false, bTest = false;
    CCommandOption oCL(argc, (const char**) argv, "c:dt");
    if (oCL.HasOption('t'))
        bTest = true;
    if (bTest)
        return Query();

    if (oCL.HasOption('c'))
        oCL.GetValue('c', strIpFilePath);
    else if (oCL.HasOption('d'))
        bDelShm = true;

    CIPPool oIPPool;
    if (bDelShm)
    {
        if (oIPPool.DeleteIpShm() != 0)
        {
            cerr << oIPPool.GetLastErrorMsg() << endl;
            return -1;
        }
    }
    else
    {
        if (oIPPool.MakeIpShm(strIpFilePath) != 0)
        {
            cerr << oIPPool.GetLastErrorMsg() << endl;
            return -1;
        }
    }
    return 0;
}
//
