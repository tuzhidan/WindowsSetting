#include <windows.h>
#include <io.h>  
#include <string>
#include <iostream>
#include <initguid.h>
#include "Convert.h"

DEFINE_GUID(PARTITION_MSFT_RESERVED_GUID, 0xE3C9E316L, 0x0B5C, 0x4DB8, 0x81, 0x7D, 0xF9, 0x2D, 0xF0, 0x02, 0x15, 0xAE);
DEFINE_GUID(PARTITION_BASIC_DATA_GUID, 0xEBD0A0A2L, 0xB9E5, 0x4433, 0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7);

bool  MBR2GPT(int  PhysicalDriveNumber,FILE *fp)
{
    // TODO: Add your control notification handler code here
    BOOL  br = FALSE;
    HANDLE  hDev = NULL;
    char  strDev[24];
	GUID guid;
	DWORD signature;
    DWORD  returnedCount = 0;
	sprintf(strDev, "\\\\.\\PhysicalDrive%d", PhysicalDriveNumber);
    if (strlen(strDev) != 0)
    {
        hDev = ::CreateFile(strDev, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDev != INVALID_HANDLE_VALUE)
        {
            DISK_GEOMETRY_EX* pDgEx = (DISK_GEOMETRY_EX*)  new  BYTE[512];
            //Get disk size
            if (::DeviceIoControl(hDev,
                IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                NULL,
                0,
                pDgEx,
                512,
                &returnedCount,
                NULL))
            {
                CREATE_DISK  cd;
                CREATE_DISK* pCd = &cd;
                PDISK_PARTITION_INFO  pDpi = (PDISK_PARTITION_INFO)pDgEx->Data;    //DiskGeometryGetPartition(pDgEx);
                if (pDpi->PartitionStyle == PARTITION_STYLE_GPT) {
                    if(fp == NULL)
                        printf("%s is already GPT style,will not convert.\n\n", strDev);
                    else
                        fprintf(fp,"%s is already GPT style,will not convert.\n\n", strDev);
                    return true;
                }
                //重新计算磁盘总大小会比直接查询的小，导致创建分区时未达到100%
                /*LARGE_INTEGER  totalSizeOfDisk;
                totalSizeOfDisk.QuadPart = pDgEx->Geometry.Cylinders.QuadPart\
                    * pDgEx->Geometry.TracksPerCylinder\
                    * pDgEx->Geometry.SectorsPerTrack\
                    * pDgEx->Geometry.BytesPerSector;*/

				CoCreateGuid(&guid);
                pCd->PartitionStyle = PARTITION_STYLE_GPT;
                pCd->Gpt.DiskId = guid;
                pCd->Gpt.MaxPartitionCount = 128;
				signature = (DWORD)time(NULL);     //get signature from current time
				pCd->Mbr.Signature = signature;
                //Initialize disk.
                if (::DeviceIoControl(hDev,
                    IOCTL_DISK_CREATE_DISK,
                    pCd,
                    sizeof(CREATE_DISK),
                    NULL,
                    0,
                    &returnedCount,
                    NULL))
                {
                    int  layoutInfoSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 20;
                    DRIVE_LAYOUT_INFORMATION_EX* lpDiskPartinfo_ex = (DRIVE_LAYOUT_INFORMATION_EX*)  new  BYTE[layoutInfoSize];
                    
                    /************************************************************************/
                    /* GPT                                                                     */
                    /************************************************************************/
					lpDiskPartinfo_ex->PartitionStyle = PARTITION_STYLE_GPT;
					lpDiskPartinfo_ex->PartitionCount = 1;

                    //LARGE_INTEGER  usedDisk;
                    //usedDisk.QuadPart = 512 + 33 * 512 + 33 * 512;  //MBR + 主分区表的大小 + 备用分区表的大小

                    lpDiskPartinfo_ex->Gpt.DiskId = pCd->Gpt.DiskId;
                    lpDiskPartinfo_ex->Gpt.MaxPartitionCount = 128;
                    lpDiskPartinfo_ex->Gpt.StartingUsableOffset.QuadPart = 2048 * 512;
                    lpDiskPartinfo_ex->Gpt.UsableLength.QuadPart = pDgEx->DiskSize.QuadPart - lpDiskPartinfo_ex->Gpt.StartingUsableOffset.QuadPart  - 56 * 512;

					/*CoCreateGuid(&guid);
                    lpDiskPartinfo_ex->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_GPT;
                    lpDiskPartinfo_ex->PartitionEntry[0].StartingOffset.QuadPart = lpDiskPartinfo_ex->Gpt.StartingUsableOffset.QuadPart;
                    lpDiskPartinfo_ex->PartitionEntry[0].PartitionLength.QuadPart = 128 * 1024 * 1024;
                    lpDiskPartinfo_ex->PartitionEntry[0].PartitionNumber = 1;
                    lpDiskPartinfo_ex->PartitionEntry[0].RewritePartition = TRUE;

                    lpDiskPartinfo_ex->PartitionEntry[0].Gpt.PartitionType = PARTITION_MSFT_RESERVED_GUID;
                    lpDiskPartinfo_ex->PartitionEntry[0].Gpt.PartitionId = guid;
                    lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Attributes = 0;
					WCHAR  Msr_Name[36] = L"ctyun gpt msr partition";
					memset(lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Name, 0, 72);
                    wcscpy(lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Name, Msr_Name);*/

					CoCreateGuid(&guid);
					lpDiskPartinfo_ex->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_GPT;
					lpDiskPartinfo_ex->PartitionEntry[0].StartingOffset.QuadPart = 2048 * 512;
                    lpDiskPartinfo_ex->PartitionEntry[0].PartitionLength.QuadPart = lpDiskPartinfo_ex->Gpt.UsableLength.QuadPart;
					lpDiskPartinfo_ex->PartitionEntry[0].PartitionNumber = 1;
					lpDiskPartinfo_ex->PartitionEntry[0].RewritePartition = TRUE;
					lpDiskPartinfo_ex->PartitionEntry[0].Gpt.PartitionType = PARTITION_BASIC_DATA_GUID;
					lpDiskPartinfo_ex->PartitionEntry[0].Gpt.PartitionId = guid;
					lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Attributes = 0;
					WCHAR  Data_Name[36] = L"ctyun gpt data partition";
					memset(lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Name, 0, 72);
					wcscpy(lpDiskPartinfo_ex->PartitionEntry[0].Gpt.Name, Data_Name);
					//Partitioning disk
                    if (::DeviceIoControl(hDev, IOCTL_DISK_SET_DRIVE_LAYOUT_EX, lpDiskPartinfo_ex, layoutInfoSize,
                        NULL, 0, &returnedCount, NULL))
                    {
                        //Verify
                        /*VERIFY_INFORMATION vi;
                        VERIFY_INFORMATION* pVi = &vi;
                        pVi->StartingOffset.QuadPart = 2048 * 512;
                        pVi->Length = (ULONG)pDgEx->DiskSize.QuadPart;
                        if (::DeviceIoControl(hDev, IOCTL_DISK_VERIFY, pVi, sizeof(VERIFY_INFORMATION),
                            NULL, 0, &returnedCount, NULL))
                        {*/
                            //Updating
                            if (::DeviceIoControl(hDev, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0,
                                NULL, 0, &returnedCount, NULL))
                            {
                                br = TRUE;
                            }
                        //}
                    }
                    delete  lpDiskPartinfo_ex;
                }
            }
            delete  pDgEx;
            ::CloseHandle(hDev);
        }
    }
    //return br;
    if (!br) {
        if(fp == NULL)
            printf("%s convert MBR to GPT failed.\n\n", strDev);
        else
            fprintf(fp,"%s convert MBR to GPT failed.\n\n", strDev);
        return  false;
    }
    if(fp == NULL)
        printf("%s convert MBR to GPT success.\n\n", strDev);
    else
        fprintf(fp,"%s convert MBR to GPT success.\n\n", strDev);
    return  true;
}

