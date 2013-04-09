#include <string>
#include <vector>
#include <algorithm>   
#include <functional>   
   
using namespace std;
 
 
inline string&  lTrim(string   &ss)   
{   
        string::iterator   p=find_if(ss.begin(),ss.end(),not1(ptr_fun(isspace)));   
            ss.erase(ss.begin(),p);   
                return  ss;   
}   
 
inline  string&  rTrim(string   &ss)   
{   
        string::reverse_iterator  p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(isspace)));   
            ss.erase(p.base(),ss.end());   
                return   ss;   
}   
 
inline   string&   trim(string   &st)   
{   
        lTrim(rTrim(st));   
            return   st;   
}   

