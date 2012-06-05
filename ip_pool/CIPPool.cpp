#include <cstring>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CIPPool.h"

namespace BCDataService
{
    namespace QQApi
    {
        using namespace std;

        int32_t CIPPool::MakeIpShm(const std::string& strIpFilePath)
        {
            ifstream ifIn(strIpFilePath.c_str());
            if (!ifIn)
            {
                //		cerr<<"open file error"<<endl;
                this->m_strErrorMsg = "open file: " + strIpFilePath + " error";
                return -1;
            }
            char buf[256];
            vector<IPInfo> vecIp;

            while (!ifIn.eof())
            {
                uint32_t dwStart, dwEnd;
                string strCountry, strProvince, strCity;
                memset(buf, 0, sizeof (buf));
                ifIn.getline(buf, sizeof (buf));
                if (strlen(buf) == 0)
                    continue;
                stringstream ssT;
                ssT.str(buf);
                ssT >> dwStart >> dwEnd >> strCountry >> strProvince >> strCity;
                IPInfo ipT;
                ipT.dwStart = dwStart;
                ipT.dwEnd = dwEnd;
                strncpy(ipT.sProvince, strProvince.c_str(), strProvince.length());
                ipT.sProvince[strProvince.length()] = '\0';
                strncpy(ipT.sCity, strCity.c_str(), strCity.length());
                ipT.sCity[strCity.length()] = '\0';
                vecIp.push_back(ipT);
            }
            ifIn.close();
            assert(!vecIp.empty());
            std::sort(vecIp.begin(), vecIp.end(), IPInfoCmpr());
            this->m_dwEntryNum = vecIp.size();
            if (this->m_dwEntryNum <= 0)
            {
                this->m_strErrorMsg = "empty file?";
                return -1;
            }

            int shmid = shmget(this->m_kShmKey, (this->m_dwEntryNum + 5) * sizeof (IPInfo), IPC_CREAT | IPC_EXCL | 0660);
            if (shmid == -1)
            {
                //                perror("shmget error");
                //                assert(0);
                this->m_strErrorMsg = string("shmget fail: ") + strerror(errno);
                return -1;
            }

            void* shmaddr = shmat(shmid, 0, 0);
            if (shmaddr == (void*) (-1))
            {
                //                perror("shmat error");
                //                assert(0);
                this->m_strErrorMsg = string("shmat fail: ") + strerror(errno);
                return -1;
            }
            memset(shmaddr, 0, (this->m_dwEntryNum + 5) * sizeof (IPInfo));
            IPInfo* pArr = this->m_pArrBegin = ((IPInfo*) shmaddr) + 1;
            memcpy(shmaddr, &(this->m_dwEntryNum), sizeof (this->m_dwEntryNum));
            for (vector<IPInfo>::const_iterator it = vecIp.begin(); it != vecIp.end(); ++it)
                //                memcpy(pArr++, &(*it), sizeof (IPInfo));
                *(pArr++) = *it; //bit to bit copy?
            return 0;
        }

        int32_t CIPPool::DeleteIpShm()
        {
            int shmid = shmget(this->m_kShmKey, 0, 0660);
            if (shmid == -1)
            {
                //                perror("shmget error");
                //                assert(0);
                this->m_strErrorMsg = string("shmget fail: ") + strerror(errno);
                return -1;
            }

            if (shmctl(shmid, IPC_RMID, 0) == -1)
            {
                this->m_strErrorMsg = string("shmctl fail: ") + strerror(errno);
                return -1;
            }
            return 0;
        }

        int32_t CIPPool::SearchInit()
        {
            int shmid = shmget(this->m_kShmKey, 0, 0660);
            if (shmid == -1)
            {
                //                perror("shmget error");
                //                assert(0);
                this->m_strErrorMsg = string("shmget fail: ") + strerror(errno);
                return -1;
            }

            void* shmaddr = shmat(shmid, 0, SHM_RDONLY);
            if (shmaddr == (void*) (-1))
            {
                //                perror("shmat error");
                //                assert(0);
                this->m_strErrorMsg = string("shmat fail: ") + strerror(errno);
                return -1;
            }

            this->m_pArrBegin = ((IPInfo*) shmaddr) + 1;
            this->m_dwEntryNum = *((int32_t*) shmaddr);
            if (this->m_dwEntryNum <= 0)
            {
                this->m_strErrorMsg = "empty file?";
                return -1;
            }

            return 0;
        }

        int32_t CIPPool::DoSearch(IPInfo *pIPInfo, const std::string& strIp)
        {
            struct in_addr in;
            if (inet_aton(strIp.c_str(), &in) == 0)
            {
                this->m_strErrorMsg = "invalid ip: " + strIp;
                return -1;
            }

            uint32_t dwLocalIp = ntohl(in.s_addr);
            IPInfo tInfo;
            tInfo.dwStart = dwLocalIp;
            IPInfo* ptInfo = std::lower_bound(this->m_pArrBegin, this->m_pArrBegin + this->m_dwEntryNum, tInfo, IPInfoCmpr());
            if (ptInfo == this->m_pArrBegin + this->m_dwEntryNum)
            {
                if (tInfo.dwStart > (ptInfo - 1)->dwEnd)
                    return 1;
                *pIPInfo = *(ptInfo - 1);
            }
            else if (ptInfo == this->m_pArrBegin)
            {
                if (tInfo.dwStart < ptInfo->dwStart)
                    return 1;
                *pIPInfo = *(ptInfo);
            }
            else
            {
                if (tInfo.dwStart == ptInfo->dwStart)
                    *pIPInfo = *(ptInfo);
                else if (tInfo.dwStart <= (ptInfo - 1)->dwEnd)
                    *pIPInfo = *(ptInfo - 1);
                else
                    return 1;
            }
            return 0;
        }

        int32_t CIPPool::SearchDone()
        {
            if (shmdt((void*) (this->m_pArrBegin - 1)) == -1)
            {
                this->m_strErrorMsg = string("shmdt fail: ") + strerror(errno);
                return -1;
            }
            return 0;
        }
    }
}
