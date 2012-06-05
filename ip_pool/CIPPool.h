/* 
 * File:   CIPPool.h
 * Author: HEME
 *
 * Created on 2012年4月17日, 上午10:26
 */

#ifndef CIPPOOL_H
#define	CIPPOOL_H
#include <string>

#include <stdint.h>
#include <sys/shm.h>

namespace BCDataService
{
    namespace QQApi
    {

        class CIPPool
        {
        public:

            struct IPInfo
            {
                uint32_t dwStart;
                uint32_t dwEnd;
                char sProvince[20];
                char sCity[50];
            };
        public:

            CIPPool() : m_kShmKey(20120417)
            {

            };

            virtual ~CIPPool()
            {

            };

            int32_t MakeIpShm(const std::string& strIpFilePath);
            int32_t DeleteIpShm();

            int32_t SearchInit();
            int32_t DoSearch(IPInfo *pIPInfo, const std::string& strIp);
            int32_t SearchDone();

            const std::string& GetLastErrorMsg()
            {
                return this->m_strErrorMsg;
            }
        private:

            struct IPInfoCmpr : public std::binary_function<IPInfo, IPInfo, bool>
            {

                bool operator()(const IPInfo& oa, const IPInfo & ob)
                {
                    return oa.dwStart < ob.dwStart;
                }
            };
        private:
            key_t m_kShmKey;
            std::string m_strErrorMsg;
            IPInfo *m_pArrBegin;
            int32_t m_dwEntryNum;
        };
    }
}


#endif	/* CIPPOOL_H */

