#ifndef BCNUM_H
#include <cstring>
#include <string>
#include <exception>
//a class for high precision compute
//
class BcNum
{
    private:
        enum sign
        {
            PLUS, 
            MINUS
        };
    public:
        class CIllegalFloatRepresentaionException : public std::exception
        {
            const char* what()
            {
                return "illegal float representation";
            }
        };
        BcNum(const BcNum& oRh);
        explicit BcNum(const std::string& strRep);
        virtual ~BcNum();
        BcNum& operator=(const BcNum& oRh);
        BcNum& operator+=(const BcNum& oRh);
        BcNum& operator-=(const BcNum& oRh);
        BcNum& operator*=(const BcNum& oRh);
        BcNum& operator/=(const BcNum& oRh);
        int Compare(const BcNum& oRh);
    private:
        int DoCompare(const BcNum& oRh, bool bUseSign, bool bIngoreLast);
        int TrimLeadingZeros();
        int AllocMem(int dwLen, int dwScale);
        bool CheckRepFormat(const std::string& strRep);
    private:
        BcNum::sign m_eSign;
        int m_dwLen;
        int m_dwScale;
        char *m_pData;
        char *m_pValue;
};

const BcNum& operator+(const BcNum& oRh);
const BcNum& operator-(const BcNum& oRh);
const BcNum& operator+(const BcNum& oLh, const BcNum& oRh);
const BcNum& operator-(const BcNum& oLh, const BcNum& oRh);
const BcNum& operator*(const BcNum& oLh, const BcNum& oRh);
const BcNum& operator/(const BcNum& oLh, const BcNum& oRh);
bool operator==(const BcNum& oLh, const BcNum& oRh);
bool operator!=(const BcNum& oLh, const BcNum& oRh);
bool operator>(const BcNum& oLh, const BcNum& oRh);
bool operator<(const BcNum& oLh, const BcNum& oRh);

extern const BcNum BN_ZERO;
extern const BcNum BN_ONE;
extern const BCNum BN_TWO;

#define BCNUM_H 
#endif /* BCNUM_H */
