#include <Windows.h>
#include <locale.h>
#include <string>

//可移植版本 wstring => string
std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, "");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = wcstombs(NULL, _Source, 0) + 1;
    char* _Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);
    wcstombs(_Dest, _Source, _Dsize);
    std::string result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

//可移植版本 string => wstring
std::wstring s2ws(const std::string& s)
{
    std::string curLocale = setlocale(LC_ALL, "");
    const char* _Source = s.c_str();
    size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest, _Source, _Dsize);
    std::wstring result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

/**判断str1是否以str2开头
* 如果是返回1
* 不是返回0
* 出错返回-1
* */
int is_begin_with(const char* str1, char* str2)
{
    if (str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    char* p = str2;
    int i = 0;
    while (*p != '\0')
    {
        if (*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

/**判断str1是否以str2结尾
* 如果是返回1
* 不是返回0
* 出错返回-1
* */
int is_end_with(const char* str1, char* str2)
{
    if (str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    while (len2 >= 1)
    {
        if (str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}