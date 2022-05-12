// DiskConvert.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <io.h>  
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Convert.h"

using namespace std;
#define GB(x) (x.HighPart << 2) + (x.LowPart >> 20) / 1024.0

// 挂载点与盘符对应关系
map<string, string> volumes;
map<string, string>::iterator iter;

void InitDiskVolumes() {
    int DiskCount = 0;
    CHAR szVolumeName[MAX_PATH] = { 0 };
    CHAR logic_dirve[5] = { 0 };
    DWORD temp = GetLogicalDrives();
    while (temp != 0) {
        if (temp & 1) {
            sprintf_s(logic_dirve, "%c:\\", 'A' + DiskCount);
            bool ret = GetVolumeNameForVolumeMountPoint(logic_dirve, szVolumeName,MAX_PATH);
            volumes.insert(pair<string, string>(szVolumeName,logic_dirve));
        }
        temp = temp >> 1;
        DiskCount++;
    }
}

bool isBlankDisk(std::string& disk)
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

bool PartitionEmpty(GUID guid) {
    char guid_str[MAX_PATH] = { 0 };
    sprintf(guid_str, "\\\\?\\Volume{%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}\\",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    printf("%-10s : %s\n", "Volume", guid_str);
    iter = volumes.find(guid_str);
    if (iter == volumes.end()) {
        printf("%-10s : %s\n", "VolumeName", "-");
        printf("%-10s : %s\n", "Is Empty", "Yes");
        return true;
    }
    else {
        string logicDrive = iter->second.c_str();
        printf("%-10s : %s\n", "VolumeName", logicDrive.c_str());
        bool isEmpty = isBlankDisk(logicDrive);
        printf("%-10s : %s\n", "Is Empty", isEmpty ? "Yes" : "No");
        return isEmpty;
    }
}

/**
    检查物理磁盘是否需要转换成GPT
    条件：
        1、原来磁盘类型为MBR
        2、原来磁盘所有分区下均无用户文件/目录
**/
bool isBlankDrive(int diskIndex)
{
    CHAR szDiskPos[24];
    bool isEmptyDrive = true;
    sprintf(szDiskPos, "\\\\.\\PhysicalDrive%d", diskIndex);
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	int nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 3 * sizeof(PARTITION_INFORMATION_EX);;
    DRIVE_LAYOUT_INFORMATION_EX* lpDiskPartinfo_ex = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(nDiskBufferSize);
	DWORD nDiskBytesRead = 0;

	memset(lpDiskPartinfo_ex, 0, sizeof(lpDiskPartinfo_ex));

	hDevice = CreateFile(szDiskPos, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	BOOL Ret = FALSE;
	Ret = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, lpDiskPartinfo_ex, nDiskBufferSize, &nDiskBytesRead, NULL);

	if (!Ret)
	{
		int e = GetLastError();
		cout << strerror(e) << endl;
		return false;
	}
    
    bool isMbr = true;
    if (lpDiskPartinfo_ex->PartitionStyle == 1) {//GPT格式
        isMbr = false;
    }
    printf("%s : %s\n", szDiskPos, isMbr ? "MBR" : "GPT");
    cout << "--------------------------------------------------------------" << endl;
    for (int i = 0;; i++) {
        PARTITION_INFORMATION_EX partition = lpDiskPartinfo_ex->PartitionEntry[i];
        if (partition.PartitionNumber > 0 && partition.PartitionLength.QuadPart > 0) {// 第一个分区为隐藏分区
            printf("%-10s : %d\n", "Partition", partition.PartitionNumber);
            printf("%-10s : %lld\n", "Size", partition.PartitionLength.QuadPart);
            if (isMbr) {
                GUID guid = partition.Mbr.PartitionId;
                isEmptyDrive &= PartitionEmpty(guid);
            }else{
                GUID guid = partition.Gpt.PartitionId;
                isEmptyDrive &= PartitionEmpty(guid);
            }
            cout << "--------------------------------------------------------------" << endl;
        }
        else {
            break;
        }
    }

	free(lpDiskPartinfo_ex);
	lpDiskPartinfo_ex = NULL;
	CloseHandle(hDevice);
    
    return isEmptyDrive;
}

void PhysicalDrives() {
    HKEY    hKEY;
    long    lRet;
    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum",
        0,
        KEY_READ,
        &hKEY);
    if (lRet != ERROR_SUCCESS) {
        printf("获取磁盘数量失败.");
        return;
    }

    int        iSize = 0;
    DWORD    dwType;
    DWORD    dwValue;
    DWORD    dwBufLen = sizeof(DWORD);
    __try {
        lRet = ::RegQueryValueEx(hKEY, "Count", NULL, &dwType, (BYTE*)&dwValue, &dwBufLen);
        if (lRet != ERROR_SUCCESS)
        {
            printf("获取磁盘数量失败..");
            __leave;
        }
        for (UINT i = 0; i < dwValue; i++)
        {
            bool empty = isBlankDrive(i);
            
            if (empty) {
                printf("\\\\.\\PhysicalDrive%d is empty,begin to convert style\n\n\n", i);
                //MBR2GPT(i);
            }
            else {
                printf("\\\\.\\PhysicalDrive%d is not empty,can't convert style\n\n\n", i);
            }
        }
    }
    __finally {
        if (hKEY != NULL) {
            RegCloseKey(hKEY);
        }
    }
}

int main(int argc,char* argvs[])
{
    InitDiskVolumes();
    if (argc == 1) {
        //PhysicalDrives();
    }
    else {
        int disk_index = atoi(argvs[1]);
        bool empty = isBlankDrive(disk_index);

        if (empty) {
            printf("\\\\.\\PhysicalDrive%d is empty,begin to convert style\n\n\n", disk_index);
            MBR2GPT(disk_index);
        }
        else {
            printf("\\\\.\\PhysicalDrive%d is not empty,can't convert style\n\n\n", disk_index);
        }
    }
    
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
