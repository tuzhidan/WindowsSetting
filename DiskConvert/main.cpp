// DiskConvert.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <io.h>  
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <comdef.h>
#include <Wbemidl.h>
#include <locale>
#include <codecvt>
#include <algorithm>

#pragma comment(lib, "wbemuuid.lib")
#include "Convert.h"

#pragma warning(disable: 4996) 

using namespace std;
#define GB(x) (x.HighPart << 2) + (x.LowPart >> 20) / 1024.0

// 挂载点与盘符对应关系
map<wstring, vector<wstring>> disks;
map<wstring, vector<wstring>>::iterator iter;
FILE* fp;
const char* log_file = "C:\\Users\\Public\\Documents\\mirror\\disk_convert.log";

//convert string to wstring
inline std::wstring to_wide_string(const std::string& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input);
}

//convert wstring to string 
inline std::string to_byte_string(const std::wstring& input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}

//void InitDiskVolumes() {
//    int DiskCount = 0;
//    CHAR szVolumeName[MAX_PATH] = { 0 };
//    CHAR logic_dirve[5] = { 0 };
//    DWORD temp = GetLogicalDrives();
//    while (temp != 0) {
//        if (temp & 1) {
//            sprintf_s(logic_dirve, "%c:\\", 'A' + DiskCount);
//            bool ret = GetVolumeNameForVolumeMountPoint(logic_dirve, szVolumeName,MAX_PATH);
//            volumes.insert(pair<string, string>(szVolumeName,logic_dirve));
//        }
//        temp = temp >> 1;
//        DiskCount++;
//    }
//}

bool isBlankDrive(std::string& disk)
{
    //是否有文件
    bool isBlank = true;
    //文件句柄  
    intptr_t hFile = 0;
    //文件信息，_finddata_t需要io.h头文件  
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(disk).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, "System Volume Information") == 0) {
                    continue;
                }
                else if (strcmp(fileinfo.name, "$RECYCLE.BIN") == 0) {
                    continue;
                }
                else {
                    isBlank = false;
                    break;
                }
            }
            else  //如果不是,加入列表  
            {
                isBlank = false;
                break;
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    return isBlank;
}

bool isBlankDisk(int diskIndex) {
    CHAR szDiskPos[24];
    sprintf(szDiskPos, "\\\\.\\PHYSICALDRIVE%d", diskIndex);

    wstring disk = to_wide_string(szDiskPos);
    iter = disks.find(disk);
    if (iter == disks.end()) {
        fprintf(fp,"%-10s : %s\n", "Drives", "-");
        fprintf(fp,"%-10s : %s\n", "Empty", "Yes");
        return true;
    }
    else {
        vector<wstring> logicDrives = iter->second;
        fprintf(fp,"%-10s : ", "Drives");
        for (vector<wstring>::iterator it = logicDrives.begin(); it != logicDrives.end(); it++)
        {
            fprintf(fp,"%ws\\ ",it->c_str());
        }
        fprintf(fp,"\n");
        bool isEmpty = true;
        for (size_t i = 0; i < logicDrives.size();i++) {
            wstring logicDrive = logicDrives.at(i);
            string drive = to_byte_string(logicDrive);
            isEmpty &= isBlankDrive(drive);
        }
        fprintf(fp,"%-10s : %s\n", "Empty", isEmpty ? "Yes" : "No");
        return isEmpty;
    }
}

void PrintGuid(GUID guid) {
    char guid_str[MAX_PATH] = { 0 };
    sprintf(guid_str, "\\\\?\\Volume{%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}\\",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    fprintf(fp,"%-10s : %s\n", "Guid", guid_str);
}

/**
* 打印磁盘分区信息
**/
void PartitionInfo(int diskIndex)
{
    CHAR szDiskPos[24];
    sprintf(szDiskPos, "\\\\.\\PhysicalDrive%d", diskIndex);
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	int nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 4 * sizeof(PARTITION_INFORMATION_EX);;
    DRIVE_LAYOUT_INFORMATION_EX* lpDiskPartinfo_ex = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(nDiskBufferSize);
	DWORD nDiskBytesRead = 0;

	memset(lpDiskPartinfo_ex, 0x00, sizeof(lpDiskPartinfo_ex));

	hDevice = CreateFileA(szDiskPos, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	BOOL Ret = FALSE;
	Ret = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, lpDiskPartinfo_ex, nDiskBufferSize, &nDiskBytesRead, NULL);

	if (!Ret)
	{
		int e = GetLastError();
		cout << strerror(e) << endl;
		return;
	}
    
    bool isMbr = true;
    if (lpDiskPartinfo_ex->PartitionStyle == PARTITION_STYLE_GPT) {//GPT格式
        isMbr = false;
    }
    fprintf(fp,"%s : %s\n", szDiskPos, isMbr ? "MBR" : "GPT");
    fprintf(fp, "----------------------------------------------------------------\n");
    for (int i = 0; i < lpDiskPartinfo_ex->PartitionCount; i++) {
        PARTITION_INFORMATION_EX partition = lpDiskPartinfo_ex->PartitionEntry[i];
        if (partition.PartitionNumber <= 0) {
            continue;
        }
        fprintf(fp,"%-10s : %d\n", "Partition", partition.PartitionNumber);
        fprintf(fp,"%-10s : %lld\n", "Size", partition.PartitionLength.QuadPart);
        if (isMbr) {
            GUID guid = partition.Mbr.PartitionId;
            PrintGuid(guid);
        }else{
            GUID guid = partition.Gpt.PartitionId;
            PrintGuid(guid);
        }
        fprintf(fp,"----------------------------------------------------------------\n");
    }

	free(lpDiskPartinfo_ex);
	lpDiskPartinfo_ex = NULL;
	CloseHandle(hDevice);
}

void PhysicalDrives() {
    HKEY    hKEY;
    long    lRet;
    lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum",
        0,
        KEY_READ,
        &hKEY);
    if (lRet != ERROR_SUCCESS) {
        fprintf(fp,"获取磁盘数量失败.\n");
        return;
    }

    int        iSize = 0;
    DWORD    dwType;
    DWORD    dwValue;
    DWORD    dwBufLen = sizeof(DWORD);
    __try {
        lRet = ::RegQueryValueExA(hKEY, "Count", NULL, &dwType, (BYTE*)&dwValue, &dwBufLen);
        if (lRet != ERROR_SUCCESS)
        {
            fprintf(fp,"获取磁盘数量失败..\n");
            __leave;
        }
        for (UINT i = 0; i < dwValue; i++)//从2块磁盘开始处理，避免无转第1块磁盘导致无法启动
        {
            PartitionInfo(i);
            bool empty = isBlankDisk(i);
            if (empty) {
                fprintf(fp,"\\\\.\\PhysicalDrive%d is empty,begin to convert style\n", i);
                if (i == 0) {
                    fprintf(fp,"skip first disk ...");
                }
                else {
                    MBR2GPT(i,fp);
                }
            }
            else {
                fprintf(fp,"\\\\.\\PhysicalDrive%d is not empty,can't convert style\n\n\n", i);
            }
        }
    }
    __finally {
        if (hKEY != NULL) {
            RegCloseKey(hKEY);
        }
    }
}

int initDiskDriveRelation()
{
    HRESULT hres;
    char WMI_SQL[MAX_PATH] = { 0 };

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Failed to initialize COM library. Error code = %ld\n", hres);
        return 1;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Failed to initialize security. Error code = %ld\n", hres);
        CoUninitialize();
        return 1;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Failed to create IWbemLocator object. Error code = %ld\n", hres);
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices* pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Could not connect. Error code = %ld\n", hres);
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    //cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Could not set proxy blanket. Error code = %ld\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pDisks = NULL;
    IEnumWbemClassObject* pPartitions = NULL;
    IEnumWbemClassObject* pLogicDrives = NULL;
    //sfprintf(fp,WMI_SQL, , diskId);
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_DiskDrive"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pDisks);

    if (FAILED(hres))
    {
        cout << "Query for operating system name failed."
            << " Error code = 0x"
            << hex << hres << endl;
        fprintf(fp, "Query for operating system name failed. Error code = %ld\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;
    VARIANT vtDisk;
    while (pDisks)
    {
        vector<wstring> drives;
        HRESULT hr = pDisks->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }
        // Get the value of the Name property
        hr = pclsObj->Get(L"DeviceID", 0, &vtDisk, 0, 0);
        //wcout << " OS Name : " << vtDisk.bstrVal << endl;
        sprintf(WMI_SQL, "ASSOCIATORS OF {Win32_DiskDrive.DeviceID='%ws'} WHERE AssocClass = Win32_DiskDriveToDiskPartition", vtDisk.bstrVal);
        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t(WMI_SQL),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pPartitions);
        //VariantClear(&vtProp);
        VARIANT vtPartition;
        while (pPartitions)
        {
            HRESULT hr = pPartitions->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }
            hr = pclsObj->Get(L"DeviceID", 0, &vtPartition, 0, 0);
            //wcout << " Partitions : " << vtPartition.bstrVal << endl;

            sprintf(WMI_SQL, "ASSOCIATORS OF {Win32_DiskPartition.DeviceID='%ws'} WHERE AssocClass = Win32_LogicalDiskToPartition", vtPartition.bstrVal);
            hres = pSvc->ExecQuery(
                bstr_t("WQL"),
                bstr_t(WMI_SQL),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pLogicDrives);
            VARIANT vtDrive;
            while (pLogicDrives)
            {
                HRESULT hr = pLogicDrives->Next(WBEM_INFINITE, 1,
                    &pclsObj, &uReturn);

                if (0 == uReturn)
                {
                    break;
                }
                hr = pclsObj->Get(L"DeviceID", 0, &vtDrive, 0, 0);
                //wcout << " LogicDrive : " << vtDrive.bstrVal << endl;
                drives.push_back(vtDrive.bstrVal);
                VariantClear(&vtDrive);
            }
            VariantClear(&vtPartition);
        }
        wstring key = vtDisk.bstrVal;
        transform(key.begin(), key.end(), key.begin(), towupper);
        disks.insert(pair<wstring,vector<wstring>>(key, drives));
        pclsObj->Release();
        VariantClear(&vtDisk);
    }
    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pDisks->Release();
    pPartitions->Release();
    pLogicDrives->Release();
    CoUninitialize();

    return 0;   // Program successfully completed.
}

void printTime() {
    time_t tt = time(NULL);
    tm* t = localtime(&tt);
    fprintf(fp,"%d-%02d-%02d %02d:%02d:%02d\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
}

int main(int argc,char* argvs[])
{
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    // 在调用GerVersionEx函数前须设置OSVERSIONINFO  结构体中的dwOSVersionInfoSize 的值
    GetVersionEx(&osvi);
    
    if ((fp = fopen(log_file, "w+")) == NULL) {
        fprintf(fp,"can't open the file! \n");
        return 0;
    }

    fprintf(fp,"\n===========================================================================\n\n");
    fprintf(fp, "start time :\t"); printTime();
    fprintf(fp, "Windows build number : %d\n\n", osvi.dwBuildNumber);
    initDiskDriveRelation();
    if (disks.size() == 0) {
        fprintf(fp, "获取磁盘与盘符对应关系异常，请检查\n");
        return 0;
    }
    if (argc == 1) {
        PhysicalDrives();
    }
    else {
        int diskIndex = atoi(argvs[1]);
        PartitionInfo(diskIndex);
        bool empty = isBlankDisk(diskIndex);

        if (empty) {
            fprintf(fp, "\\\\.\\PhysicalDrive%d is empty,begin to convert style\n", diskIndex);
            if (diskIndex == 0) {
                fprintf(fp, "skip first disk ...");
            }
            else {
                MBR2GPT(diskIndex,fp);
            }
        }
        else {
            fprintf(fp, "\\\\.\\PhysicalDrive%d is not empty,can't convert style\n\n\n", diskIndex);
        }
    }
    fprintf(fp, "\nfinish time :\t"); printTime();
    fprintf(fp, "\n===========================================================================\n");
}
