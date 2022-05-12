#include <Windows.h>
#include <locale.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "Registry.h"
#include "Util.h"


std::string GetRegValue(const std::string& strUrl, const std::string& strKey)
{
    std::string strValue("");
    HKEY hKey = NULL;
    HKEY  hKeyResult = NULL;
    DWORD dwSize = 0;
    DWORD dwDataType = 0;
    std::wstring wstrUrl = s2ws(strUrl);
    std::wstring wstrKey = s2ws(strKey);
    int index = strUrl.find_first_of("\\");
 
    if (is_begin_with(strUrl.c_str(), (char*)"HKCU") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CURRENT_USER")) {
        hKey = HKEY_CURRENT_USER;
        wstrUrl = wstrUrl.substr(index + 1);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCR") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CLASSES_ROOT")) {
        hKey = HKEY_CLASSES_ROOT;
        wstrUrl = wstrUrl.substr(index + 1);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKLM") || is_begin_with(strUrl.c_str(), (char*)"HKEY_LOCAL_MACHINE")) {
        hKey = HKEY_LOCAL_MACHINE;
        wstrUrl = wstrUrl.substr(index + 1);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKU") || is_begin_with(strUrl.c_str(), (char*)"HKEY_USERS")) {
        hKey = HKEY_USERS;
        wstrUrl = wstrUrl.substr(index + 1);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCC") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CURRENT_CONFIG")) {
        hKey = HKEY_CURRENT_CONFIG;
        wstrUrl = wstrUrl.substr(index + 1);
    }
    if (hKey == 0) {
        return strValue;
    }

    //打开注册表
    long ret = RegOpenKeyEx(hKey, wstrUrl.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKeyResult);
    if (ERROR_SUCCESS == ret)
    {
        // 获取缓存的长度dwSize及类型dwDataType
        ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, NULL, &dwSize);
        switch (dwDataType)
        {
        case REG_MULTI_SZ:
        {
            //分配内存大小
            std::vector<std::wstring> target;
            std::vector<wchar_t> temp(dwSize / sizeof(wchar_t));
            //BYTE* lpValue = new BYTE[dwSize];
            //获取注册表中指定的键所对应的值
            LONG lRet = ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, reinterpret_cast<LPBYTE>(&temp[0]), &dwSize);
            size_t index = 0;
            size_t len = wcslen(&temp[0]);
            while (len > 0)
            {
                target.push_back(&temp[index]);
                index += len + 1;
                len = wcslen(&temp[index]);
            }
            for (int i = 0; i < static_cast<int>(target.size()); i++) {
                strValue = strValue.append(ws2s(target.at(i))).append("~!~");
            }
            //delete[] temp;
            break;
        }
        case REG_SZ:
        {
            //分配内存大小
            wchar_t* lpValue = new wchar_t[dwSize];
            memset(lpValue, 0, dwSize * sizeof(wchar_t));
            //获取注册表中指定的键所对应的值
            if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
            {
                std::wstring wstrValue(lpValue);
                strValue = ws2s(wstrValue);
            }
            delete[] lpValue;
            break;
        }
        case REG_DWORD:
        {
            //获取注册表中指定的键所对应的值
            DWORD lpValue;
            if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, (LPBYTE)&lpValue, &dwSize))
            {
                strValue = std::to_string(lpValue);
            }
            break;
        }
        default:
            break;
        }
    }

    //关闭注册表
    ::RegCloseKey(hKeyResult);


    return strValue;
}

std::string hexToStr(std::string str)
{
    std::string hex = str;
    long len = hex.length();
    std::string newString;
    for (long i = 0; i < len; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}

int SetRegValue(const std::string& strUrl, const std::string& strKey, const std::string& strValue,int valueType)
{
    HKEY hKey = NULL;
    HKEY  hKeyResult = NULL;
    DWORD dwDataType = valueType;
    std::wstring wstrUrl = L"";
    std::wstring wstrKey = s2ws(strKey);
    std::wstring wstrValue = s2ws(strValue);
    if (is_begin_with(strUrl.c_str(), (char*)"HKCU") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CURRENT_USER")) {
        hKey = HKEY_CURRENT_USER;
        wstrUrl = s2ws(strUrl.substr(5));
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCR") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CLASSES_ROOT")) {
        hKey = HKEY_CLASSES_ROOT;
        wstrUrl = s2ws(strUrl.substr(5));
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKLM") || is_begin_with(strUrl.c_str(), (char*)"HKEY_LOCAL_MACHINE")) {
        hKey = HKEY_LOCAL_MACHINE;
        wstrUrl = s2ws(strUrl.substr(5));
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKU") || is_begin_with(strUrl.c_str(), (char*)"HKEY_USERS")) {
        hKey = HKEY_USERS;
        wstrUrl = s2ws(strUrl.substr(4));
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCC") || is_begin_with(strUrl.c_str(), (char*)"HKEY_CURRENT_CONFIG")) {
        hKey = HKEY_CURRENT_CONFIG;
        wstrUrl = s2ws(strUrl.substr(5));
    }
    if (hKey == 0) {
        return -1;
    }
    int ret = 0;
    ret = RegOpenKeyEx(hKey, wstrUrl.c_str(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKeyResult);
    if (ret == ERROR_FILE_NOT_FOUND) {
        ret = RegCreateKey(hKey, wstrUrl.c_str(),&hKeyResult);
    }
    if (ret == ERROR_SUCCESS)
    {
        if (REG_BINARY == valueType) {
            std::string str = std::string(strValue);
            str = boost::regex_replace(str, boost::regex(" "), "");
            str = hexToStr(str);
            ret = RegSetValueEx(hKeyResult, (LPCWSTR)wstrKey.c_str(), 0, REG_BINARY, (unsigned char*)str.c_str(), str.length());
        }
        else if(REG_DWORD == valueType){
            char* str;
            long dwordValue = strtol(strValue.c_str(), &str, 16);
            ret = RegSetValueExA(hKeyResult, (LPCSTR)strKey.c_str(), 0, REG_DWORD, (CONST BYTE*) & dwordValue, sizeof(DWORD));
        }
        else if (REG_EXPAND_SZ == valueType) {
            boost::algorithm::trim_if(wstrValue, boost::algorithm::is_any_of("\""));
            int wlen = wcslen(wstrValue.c_str()) * 2;
            char* pElementText = new char[wlen];
            WideCharToMultiByte(CP_ACP, NULL, wstrValue.c_str(), -1, pElementText, wlen + 2, NULL, NULL);
            ret = RegSetValueExA(hKeyResult, (LPCSTR)strKey.c_str(), 0, REG_EXPAND_SZ, (unsigned char*)pElementText, wlen);
        }
        else if (REG_MULTI_SZ == valueType) {
            std::vector<std::wstring> target;
            std::string temp;
            boost::split_regex(target, wstrValue, boost::regex("~!~"));
            int size = 0;
            for (int i = 0; i < static_cast<int>(target.size());i++) {
                int wlen = wcslen(target.at(i).c_str()) * 2;
                char* pElementText = new char[wlen];
                WideCharToMultiByte(CP_ACP, NULL, target.at(i).c_str(), -1, pElementText, wlen +2, NULL, NULL);
                temp = temp + std::string(pElementText) + '\0';
            }
            ret = RegSetValueExA(hKeyResult, (LPCSTR)strKey.c_str(), 0, REG_MULTI_SZ, (unsigned char*)temp.c_str(), temp.length());
        }
        else if (REG_SZ == valueType) {
            boost::algorithm::trim_if(wstrValue, boost::algorithm::is_any_of("\""));
            int wlen = wcslen(wstrValue.c_str()) * 2;
            char* pElementText = new char[wlen];
            WideCharToMultiByte(CP_ACP, NULL, wstrValue.c_str(), -1, pElementText, wlen + 2, NULL, NULL);
            ret = RegSetValueExA(hKeyResult, (LPCSTR)strKey.c_str(), 0, REG_SZ, (unsigned char*)pElementText, wlen);
        }
       
        RegCloseKey(hKeyResult);
    }
    
    return 0;
}

int CreateRegKey(const std::string& strUrl)
{
    HKEY hKey = NULL;
    HKEY  hKeyResult = NULL;
    DWORD dwDisposition;
    std::string wstrUrl = strUrl;
    if (is_begin_with(strUrl.c_str(), (char*)"HKCU")) {
        hKey = HKEY_CURRENT_USER;
        wstrUrl = wstrUrl.substr(5);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCR")) {
        hKey = HKEY_CLASSES_ROOT;
        wstrUrl = wstrUrl.substr(5);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKLM")) {
        hKey = HKEY_LOCAL_MACHINE;
        wstrUrl = wstrUrl.substr(5);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKU")) {
        hKey = HKEY_USERS;
        wstrUrl = wstrUrl.substr(4);
    }
    else if (is_begin_with(strUrl.c_str(), (char*)"HKCC")) {
        hKey = HKEY_CURRENT_CONFIG;
        wstrUrl = wstrUrl.substr(5);
    }
    if (hKey == 0) {
        return -1;
    }
    if (ERROR_SUCCESS != RegCreateKeyExA(hKey, wstrUrl.c_str(),0,NULL,0, KEY_WRITE | KEY_WOW64_64KEY,NULL, &hKeyResult,&dwDisposition)) {
        return 0;
    }
    else {
        return -1;
    }
    
}
