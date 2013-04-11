//    /usr/local/bocai_home/log/my/tenpay/20130313/payNotifyUrl.log
//    /usr/local/bocai_home/log/my/tenpay/20130313/payNotifyUrl_error.log
//    /usr/local/bocai_home/log/my/tenpay/20130313/01_order_query_error.log
#include <string>
//#include <regex>

using namespace std;

string& lTrim(string& ss)
{
    string::iterator p=find_if(ss.begin(),ss.end(),not1(ptr_fun(isspace)));
    ss.erase(ss.begin(),p);
    return ss;
}

string& rTrim(string& ss)
{
    string::reverse_iterator p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(isspace)));
    ss.erase(p.base(),ss.end());
    return ss;
}

string& trim(string& st)
{
    lTrim(rTrim(st));
    return st;
}

string& RmCommonDirName(string& strIn)
{
    const strPrefix("/usr/local/bocai_home/log/");
    if (strIn.substr(0, strPrefix.length()) == strPrefix)
        strIn.erase(0, strPrefix.length());
    return strIn;
}

string& RmExtendName(string& strIn)
{
    const strSuffix(".log");
    if (strIn.substr(strIn.length() - strSuffix.length(), strSuffix.length()) == strSuffix)
        strIn.erase(strIn.length() - strSuffix.length());
    return strIn;
}

struct IsNoise: public std::unary_function<char, bool>
{
    bool operator()(const char a)
    {
        return (not ((a >= '0' and a <= '9') or (a >= 'a' and a <= 'z') or (a >= 'A' and a <= 'Z') or a = '/' or a = '_'));
    }
};

struct DupSlash : public std::binary_function<char, char, bool>
{
    bool operator()(const char a, const char b)
    {
        return (a == '/' and b == '/');
    }
};

string& UniqueSlash(string& strIn)
{
    //iterator unique( iterator start, iterator end, BinPred p );
    strIn.erase(std::unique(strIn.begin(), strIn.end(), DupSlash()), strIn.end());
    return strIn;
}

bool IsLegalYear(const string& strIn)
{
    char lsYear[][3] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", 
        "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", 
        "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", 
        "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", 
        "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", 
        "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", 
        "96", "97", "98", "99",
    };
    vector vecYear(lsYear + 0, lsYear + sizeof(lsYear) / sizeof(lsYear[0]));
    return std::find(vecYear.begin(), vecYear.end(), strIn) != vecYear.end();
}

bool IsLegalMonth(const string& strIn)
{
    char lsMon[][3] = {
        "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
    };
    vector vecMonth(lsMonth + 0, lsMonth + sizeof(lsMonth) / sizeof(lsMonth[0]));
    return std::find(vecMonth.begin(), vecMonth.end(), strIn) != vecMonth.end();
}

bool IsLegalDay(const string& strIn)
{
    char lsDay[][3] = {
        "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", 
        "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", 
    };
    vector vecDay(lsDay + 0, lsDay + sizeof(lsDay) / sizeof(lsDay[0]));
    return std::find(vecDay.begin(), vecDay.end(), strIn) != vecDay.end();
}

bool IsLegalHour(const string& strIn)
{
    char lsDay[][3] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", 
        "16", "17", "18", "19", "20", "21", "22", "23", 
    };
    vector vecHour(lsHour + 0, lsHour + sizeof(lsHour) / sizeof(lsHour[0]));
    return std::find(vecHour.begin(), vecHour.end(), strIn) != vecHour.end();
}

bool IsLegalMin(const string& strIn)
{
    char lsMin[][3] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", 
        "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", 
        "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", 
        "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", 
    };
    vector vecMin(lsMin + 0, lsMin + sizeof(lsMin) / sizeof(lsMin[0]));
    return std::find(vecMin.begin(), vecMin.end(), strIn) != vecMin.end();
}

string& RmDateTime(string& strIn)
{
//    std::tr1::regex regSlashDate("/((?:20)?[0-9]{2})?(?:0[1-9]|1[0-2])(?:[0-2][1-9]|[1-3]0|31)");
//    std::tr1::regex regSlash("/[0-1][0-9]|2[0-3]([0-5][0-9])?");
    int i = 0;
    string strTmp;
    string strPadIn = strIn + string(8, '$');
    while (i < strIn.length())
    {
        if (strPadIn[i] != '/')
        {
            strTmp.append(strPadIn[i]);
            ++i;
        }
        else
        {
            char cTemp = strPadIn[i];
            ++i;
            if (strPadIn.substr(i, 2) == "20" and IsLegalYear(strPadIn.substr(i + 2, 2)) and IsLegalMonth(strPadIn.substr(i + 4, 2)) and IsLegalDay(strPadIn.substr(i + 6, 2)))
            {
                i += 8;
            }
            else if(IsLegalYear(strPadIn.substr(i, 2)) and IsLegalMonth(strPadIn.substr(i + 2, 2)) and IsLegalDay(strPadIn.substr(i + 4, 2)))
            {
                i += 6;
            }
            else if(IsLegalMonth(strPadIn.substr(i, 2)) and IsLegalDay(strPadIn.substr(i + 2, 2)))
            {
                i += 4;
            }
            else if(IsLegalHour(strpadin.substr(i, 2)) and IsLegalMin(strPadIn.substr(i + 2, 2)))
            {
                i += 4;
            }
            else if(IsLegalHour(strpadin.substr(i, 2)))
            {
                i += 2;
            }
            else
            {
                strPadIn.append(cTemp);
                strPadIn.append(strpadin.substr(i, 2));
                i += 2;
            }
        }
    }
    strIn.swap(strTmp);
    assert(strIn.substr(strIn.length() - 8) == string(8, '$'));
    strIn.erase(strIn.length() - 8);
    return strIn;
}

string& NormalizeLogPath(string& strIn)
{
    //1,trim
    //2,unique slash
    //3,remove prefix
    //4,remove suffix
    //4,replace all char not in (0-9,a-z,A-Z,/,_) with ""
    //5,remove datetime
    //date 20120101,120101,0101
    //time 1354,14
    strIn = trim(strIn);
    strIn = UniqueSlash(strIn);
    strIn = RmCommonDirName(strIn);
    strIn = RmExtendName(strIn);
    strIn.erase(std::remove_if(strIn.begin(), strIn.end(), IsNoise()));

}

