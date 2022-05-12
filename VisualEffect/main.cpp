// VisualEffectSetting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "VisualEffect.h"

void ShowUseage() {
    std::cout << "VisualEffect.exe [option] args\n";
    std::cout << "\t list : list current setting options.\n";
    std::cout << "\t get : get current setting as parameters.\n";
    std::cout << "\t set : set visual effects by parameters.require two args\n";
    std::cout << "\t\t arg 1:setting options (0 - 3)\n";
    std::cout << "\t\t arg 2:selected options.eg: 01000100001000000\n";
}

int main(int argc,char* argv[])
{
    std::string radio;
    std::string checked;
    if (argc == 1) {
        ShowUseage();
    }
    else {
        if (strcmp(argv[1],"list") == 0) {
            GetEffect(radio, checked);
            ShowSetting(radio,checked);
        }
        else if (strcmp(argv[1],"set")== 0) {
            if (argc == 4) {
                radio = argv[2];
                checked = argv[3];
                SetEffect(radio, checked);
                checked = "";
                GetEffect(radio, checked);
                ShowSetting(radio,checked);
            }
            else {
                std::cout << "command parameter count error.";
                return 0;
            }
        }
        else if (strcmp(argv[1],"get") == 0) {
            GetEffect(radio, checked);
            std::cout << "Paramster 1:" << radio << "\n";
            std::cout << "Paramster 2:" << checked << "\n";
        }
        else {
            ShowUseage();
        }
        return 0;
    }
    
    
    //checked = "00101101001011101";
    //SetEffect("3", checked);
    return 0;
}
