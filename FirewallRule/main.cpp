// WindowsSetting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "firewall.h"
#include "comm.h"

void PrintUsage()
{
    printf("FirewallRule.exe usage:\n");
    printf("----------------------------------------------------------------------\n");
    printf("FirewallRule.exe /create [args]     创建防火墙策略\n");
    printf("\t\t arg1: in/out（字典值四选一），入规则/出规则\n");
    printf("\t\t arg2: 规则名称，出入规则名称不能相同\n");
    printf("\t\t arg3: 阻止或放行的应用程序名称/路径\n");
    printf("\t\t arg4: 规则描述（可为空）\n");
    printf("\t\t arg5: 规则所属用户组（可为空）\n");
    printf("----------------------------------------------------------------------\n");
    printf("FirewallRule.exe /delete [args]     删除防火墙策略\n");
    printf("\t\t arg1: 规则名称\n");
    printf("----------------------------------------------------------------------\n");
    printf("FirewallRule.exe /update [args]     更新防火墙策略状态\n");
    printf("\t\t arg1: 规则名称\n");
    printf("\t\t arg2: open/close/deny/allow（字典值四选一）\n");
    printf("\t\t       open\t启用规则\n");
    printf("\t\t       close\t停用规则\n");
    printf("\t\t       deny\t阻止应用\n");
    printf("\t\t       allow\t放行应用\n");
    printf("----------------------------------------------------------------------");
}

int main(int argc, char* argvs[])
{
    if (argc == 1) {
        PrintUsage();
    }
    else {
        std::cout << "--------- begin to set ctyun app's firewall policy ----------\n\n";
        if (strcmp("/create", argvs[1]) == 0) {
            if (argc >= 7) {
                char* in_out = argvs[2];
                char* ruleName = argvs[3];
                char* appPath = argvs[4];
                char* ruleDesc = argvs[5];
                char* ruleGroup = argvs[6];
                CreateFireWallPolicy(in_out,AnsiCharToWide(ruleName), AnsiCharToWide(appPath), AnsiCharToWide(ruleDesc), AnsiCharToWide(ruleGroup));
            }
            else {
                printf("not enough arguments");
            }
        }
        else if (strcmp("/delete", argvs[1]) == 0) {
            if (argc >= 3) {
                DeleteFireWallPolicy(AnsiCharToWide(argvs[2]));
            }
            else {
                printf("not enough arguments");
            }
        }
        else if (strcmp("/update", argvs[1]) == 0) {
            if (argc >= 4) {
                wchar_t* ruleName = AnsiCharToWide(argvs[2]);
                if (strcmp("Open",argvs[3]) == 0 || strcmp("open", argvs[3]) == 0) {
                    UpdateFireWallPolicy(ruleName, 1,-1);
                }
                else if (strcmp("Close", argvs[3]) == 0 || strcmp("close", argvs[3]) == 0) {
                    UpdateFireWallPolicy(ruleName, 0,-1);
                }
                else if (strcmp("Deny", argvs[3]) == 0 || strcmp("deny", argvs[3]) == 0) {
                    UpdateFireWallPolicy(ruleName, -1,1);
                }
                else if (strcmp("Allow", argvs[3]) == 0 || strcmp("allow", argvs[3]) == 0) {
                    UpdateFireWallPolicy(ruleName, -1,0);
                }
            }
            else {
                printf("not enough arguments");
            }
        }
        else {
            printf("argument error");
        }
        std::cout << "\n\n--------- set ctyun app's firewall policy finished ----------\n";
    }
    
    
    
    return 0;
}
