//    /usr/local/****_home/log/my/tenpay/20130313/payNotifyUrl.log
//    /usr/local/****_home/log/my/tenpay/20130313/payNotifyUrl_error.log
//    /usr/local/****_home/log/my/tenpay/20130313/01_order_query_error.log
#include <string>
#include <vector>
#include <cassert>

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
    const string strPrefix("/usr/local/****_home/log/");
    if (strIn.substr(0, strPrefix.length()) == strPrefix)
        strIn.erase(0, strPrefix.length());
    return strIn;
}

string& RmExtendName(string& strIn)
{
    const string strSuffix(".log");
    if (strIn.substr(strIn.length() - strSuffix.length(), strSuffix.length()) == strSuffix)
        strIn.erase(strIn.length() - strSuffix.length());
    return strIn;
}

struct IsNoise: public std::unary_function<char, bool>
{
    bool operator()(const char a)
    {
        return not ((a >= '0' and a <= '9') or (a >= 'a' and a <= 'z') or (a >= 'A' and a <= 'Z') or a == '/' or a == '_');
    }
};

struct DupUnderscore: public std::binary_function<char, char, bool>
{
    bool operator()(const char a, const char b)
    {
        return (a == '_' and b == '_');
    }
};

string& UniqueUnderscore(string& strIn)
{
    //iterator unique( iterator start, iterator end, BinPred p );
    strIn.erase(std::unique(strIn.begin(), strIn.end(), DupUnderscore()), strIn.end());
    return strIn;
}

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
    vector<string> vecYear(lsYear + 0, lsYear + sizeof(lsYear) / sizeof(lsYear[0]));
    //return std::find(vecYear.begin(), vecYear.end(), strIn) != vecYear.end();
    return std::binary_search(vecYear.begin(), vecYear.end(), strIn);
}

bool IsLegalMonth(const string& strIn)
{
    char lsMonth[][3] = {
        "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
    };
    vector<string> vecMonth(lsMonth + 0, lsMonth + sizeof(lsMonth) / sizeof(lsMonth[0]));
    //return std::find(vecMonth.begin(), vecMonth.end(), strIn) != vecMonth.end();
    return std::binary_search(vecMonth.begin(), vecMonth.end(), strIn);
}

bool IsLegalDay(const string& strIn)
{
    char lsDay[][3] = {
        "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", 
        "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", 
    };
    vector<string> vecDay(lsDay + 0, lsDay + sizeof(lsDay) / sizeof(lsDay[0]));
    //return std::find(vecDay.begin(), vecDay.end(), strIn) != vecDay.end();
    return std::binary_search(vecDay.begin(), vecDay.end(), strIn);
}

bool IsLegalHour(const string& strIn)
{
    char lsHour[][3] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", 
        "16", "17", "18", "19", "20", "21", "22", "23", 
    };
    vector<string> vecHour(lsHour + 0, lsHour + sizeof(lsHour) / sizeof(lsHour[0]));
    //return std::find(vecHour.begin(), vecHour.end(), strIn) != vecHour.end();
    return std::binary_search(vecHour.begin(), vecHour.end(), strIn);
}

bool IsLegalMin(const string& strIn)
{
    char lsMin[][3] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", 
        "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", 
        "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", 
        "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", 
    };
    vector<string> vecMin(lsMin + 0, lsMin + sizeof(lsMin) / sizeof(lsMin[0]));
    //return std::find(vecMin.begin(), vecMin.end(), strIn) != vecMin.end();
    return std::binary_search(vecMin.begin(), vecMin.end(), strIn);
}

string& RmDateTime(string& strIn)
{
//    std::tr1::regex regSlashDate("/((?:20)?[0-9]{2})?(?:0[1-9]|1[0-2])(?:[0-2][1-9]|[1-3]0|31)");
//    std::tr1::regex regSlash("/[0-1][0-9]|2[0-3]([0-5][0-9])?");
    assert(strIn.find('$') == string::npos);
    int i = 0;
    string strTmp;
    string strPadIn = strIn + string(8, '$');
    while (i < strPadIn.length())
    {
//        printf("%s\n", strTmp.c_str());
        if (not (strPadIn[i] >= '0' and strPadIn[i] <= '9'))
        {
            strTmp.push_back(strPadIn[i++]);
        }
        else
        {
//            strTmp.push_back(strPadIn[i++]);
            if (strPadIn.substr(i, 2) == "20" and IsLegalYear(strPadIn.substr(i + 2, 2)) and IsLegalMonth(strPadIn.substr(i + 4, 2)) 
                    and IsLegalDay(strPadIn.substr(i + 6, 2)) and IsLegalHour(strPadIn.substr(i + 8, 2)))
            {
                i += 10;
            }
            else if (strPadIn.substr(i, 2) == "20" and IsLegalYear(strPadIn.substr(i + 2, 2)) 
                    and IsLegalMonth(strPadIn.substr(i + 4, 2)) and IsLegalDay(strPadIn.substr(i + 6, 2)))
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
            else if(IsLegalHour(strPadIn.substr(i, 2)) and IsLegalMin(strPadIn.substr(i + 2, 2)))
            {
                i += 4;
            }
            else if(IsLegalHour(strPadIn.substr(i, 2)))
            {
                i += 2;
            }
            else
            {
                strTmp.push_back(strPadIn[i++]);
            }
        }
    }
    strIn.swap(strTmp);
    assert(strIn.substr(strIn.length() - 8) == string(8, '$'));
    strIn.erase(strIn.length() - 8, 8);
    return strIn;
}

string& RmNoise(string& strIn)
{
    strIn.erase(std::remove_if(strIn.begin(), strIn.end(), IsNoise()), strIn.end());
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
    //date 2012010115, 20120101,120101,0101
    //time 1354,14
    strIn = trim(strIn);
    strIn = UniqueSlash(strIn);
    strIn = RmCommonDirName(strIn);
    strIn = RmExtendName(strIn);
    strIn = RmNoise(strIn);
    strIn = RmDateTime(strIn);
    strIn = UniqueSlash(strIn);
    return strIn;
}

//int main(int argc, char** argv)
//{
//    char in[200];
//    while(scanf("%s", in) != EOF)
//    {
//        string strIn = string(in);
//        printf("%s\n", NormalizeLogPath(strIn).c_str());
//    }
////    if (argc < 2)
////    {
////        printf("no argv[1]\n");
////        return -1;
////    }
////    string strIn = string(argv[1]);
////    printf("%s\n", NormalizeLogPath(strIn).c_str());
//    return 0;
//}

