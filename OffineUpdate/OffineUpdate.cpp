// OffineUpdate.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <iostream>
#include <io.h>
#include <vector>
#include <sys/stat.h>

using namespace std;
char wmic[128], result[1024] = { 0 };

void initParam() {
    memset(wmic, 0x00, sizeof(wmic));
    memset(result, 0x00, sizeof(result));
}

int execmd(char* cmd, char* result, bool showInfo) {
    char buffer[1024]; //定义缓冲区
    FILE* pipe = _popen(cmd, "r"); //打开管道，并执行命令
    if (!pipe) return -1; //返回0表示运行失败

    while (!feof(pipe)) {
        if (fgets(buffer, 1024, pipe)) { //将管道输出到result中
            if (showInfo) {
                std::cout << buffer;
            }
            else {
                strcat(result, buffer);
            }
        }
    }
    int err_code = GetLastError();
    _pclose(pipe); //关闭管道
    return err_code;
}

bool isKbInstalled(char * kb) {
    initParam();
    sprintf(wmic, "wmic qfe where hotfixid=\"%s\" get hotfixid,installedon |find KB",kb);
    int bRet = execmd(wmic, result,false);
    printf(result);
    if (bRet == 1) {
        if (strncmp(result,"KB",2) == 0) {
            printf("hotfixid %s alreay installed\n", kb);
            return true;
        }
        else {
            printf("hotfixid %s has not install,ready to install it\n", kb);
            return false;
        }
    }
    else {
        printf("%s command execute error:%s\n",wmic,result);
        return false;
    }
}

/* 获取文件夹下所有文件名
输入：
    path : 文件夹路径
    exd :  所要获取的文件名后缀，如jpg、png等；如果希望获取所有文件名, exd = ""或"*"
输出：
    files : 获取的文件名列表
*/
void ListFiles(string path, string exd, vector<string>& files)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string pathName, exdName;

    if (0 != strcmp(exd.c_str(), ""))
    {
        exdName = "\\*." + exd;
    }
    else
    {
        exdName = "\\*";
    }

    if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
    {
        do
        {
            //cout << fileinfo.name << endl; 

            //如果是文件夹中仍有文件夹,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    ListFiles(pathName.assign(path).append("\\").append(fileinfo.name), exd, files);
            }
            else
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    files.push_back(pathName.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

string getFileFromPath(string path) {
    size_t pos = path.find_last_of("\\");
    return path.substr(pos + 1);
}

string getExtendFromPath(string path) {
    size_t pos = path.find_last_of(".");
    return path.substr(pos);
}

void printTime() {
    time_t tt = time(NULL);
    tm* t = localtime(&tt);
    printf("%d-%02d-%02d %02d:%02d:%02d\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
}

int main(int argc,char* argvs[])
{
    char* path = argvs[1];
    struct _stat buf;
    vector<string> cab_files;
    vector<string> msu_files;
    int failCount = 0, iRet = 0, service_status = -1;

    printf("\n===========================================================================\n\n");
    printf("start time :\t"); printTime();
    if (argc == 1) {
        printf("\n请输入补丁文件或补丁路径.\n\n");
        goto finish;
    }
    
    iRet = _stat(path, &buf);
    if (iRet == -1) {
        printf("error : %s does not exist\n",path);
        goto finish;
    }
    else if (_S_IFREG & buf.st_mode) {
        if (strcmp(getExtendFromPath(path).c_str(), ".msu") == 0) {
            msu_files.push_back(path);
        }
        else if (strcmp(getExtendFromPath(path).c_str(), ".cab") == 0) {
            cab_files.push_back(path);
        }
    }
    else {
        ListFiles(path, "cab", cab_files);
        ListFiles(path, "msu", msu_files);
    }    
    
    if (msu_files.size() > 0) {
        printf("\n-------------------------- detect wuauserv service ------------------------\n\n");
        iRet = system("sc start wuauserv");
        if (iRet == 1058) {
            // Windows update 服务被禁止
            printf("wuauserv service is disabled,config and run it\n");
            system("sc config wuauserv start= demand");
            iRet = system("sc start wuauserv");
            service_status = SERVICE_DISABLED;
        }
        else if (iRet == 1056) {
            // Windows update 服务运行中
            printf("wuauserv service is running\n");
            service_status = SERVICE_RUNNING;
        }
        else {
            printf("wuauserv service is stop,start it\n");
            service_status = SERVICE_STOP;
        }
    }
    printf("\n-------------------------- install hotfix file ----------------------------\n\n");
    for (int i = 0; i < static_cast<int>(cab_files.size());i++) {
        initParam();
        string path = cab_files.at(i);
        printf("process file : %s\n", path.c_str());
        sprintf(wmic,"dism /online /add-package /packagepath:%s /norestart", path.c_str());
        iRet = execmd(wmic,result,true);
        if (iRet == -1) {
            printf("hotfix %s installed failed.\n", path.c_str());
            failCount++;
        }
        printf("\n---------------------------------------------------------------------------\n");
    }
    
    for (int i = 0; i < static_cast<int>(msu_files.size()); i++) {
        string path = msu_files.at(i);
        printf("process file : %s\n", path.c_str());
        sprintf(wmic, "wusa %s /norestart /quiet", path.c_str());
        iRet = system(wmic);
        if (iRet == 2359302) {
            printf("hotfix %s already installed .\n", getFileFromPath(path).c_str());
        }
        else if(iRet == 3010 || iRet == 3011) {
            printf("%s install success.\n", getFileFromPath(path).c_str());
        }
        else if (iRet == -2145124329) {
            printf("%s install failure, This update is not applicable to your computer\n", getFileFromPath(path).c_str());
        }
        else{
            printf("%s install failure. error code:%d\n", getFileFromPath(path).c_str(),iRet);
            failCount++;
        }
        printf("\n---------------------------------------------------------------------------\n");
    }
    if (service_status == SERVICE_DISABLED) {
        printf("config server wuauserv disabled");
        system("net stop wuauserv");
        system("sc config wuauserv start= disabled");
    }
    else if(service_status == SERVICE_STOP) {
        system("net stop wuauserv");
    }

    printf("\nAll hotfix install finished,has %d install failed .\n\n",failCount);

finish:
    printf("finish time :\t"); printTime();

    printf("\n===========================================================================\n");
}
