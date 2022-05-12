#pragma comment ( lib, "shell32.lib " )
#include <Windows.h>
#include "VisualEffect.h"
#include "Registry.h"
#include "Util.h"
#include <iostream>

void GetEffect(std::string &radio,std::string &checked)
{
    radio = GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects", "VisualFXSetting");
    BOOL bolValue = FALSE;
    for (int index = 0; index < 17; index++) {
        switch (index) {
        case 0:
            //0 = 口保存任务栏缩略图预览
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "AlwaysHibernateThumbnails"));
            break;
        case 1:
            //1=口窗口内的动画控件和元素
            ::SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 2:
            //2 = 口淡入淡出或滑动菜单到视图
            ::SystemParametersInfo(SPI_GETMENUANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 3:
            //3=口滑动打开组合框
            ::SystemParametersInfo(SPI_GETCOMBOBOXANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 4:
            //4=口平滑滚动列表框
            ::SystemParametersInfo(SPI_GETLISTBOXSMOOTHSCROLLING, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 5:
            //5=口平滑屏荨字体边缘
            ::SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &bolValue, 0);
            checked.append(std::to_string(bolValue));
            break;
        case 6:
            //6=口启用速览
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "EnableAeroPeek"));
            break;
        case 7:
            //7=口任务栏中的动画
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "TaskbarAnimations"));
            break;
        case 8:
            //8=口拖动时显示窗口内容
            ::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bolValue, 0);
            checked.append(std::to_string(bolValue));
            break;
        case 9:
            //9=口显示缩略图，而不是显示图标:注册表项太多，暂定只有选择最佳性能时才勾选
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "IconsOnly") == "1" ? "0" : "1");
            break;
        case 10:
            //10=口显示亚透明的选择长方形
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewAlphaSelect"));
            break;
        case 11:
            //11=口在窗口下显示阴影
            ::SystemParametersInfo(SPI_GETDROPSHADOW, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 12:
            //12 = 口在单击后淡出菜单
            ::SystemParametersInfo(SPI_GETSELECTIONFADE, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 13:
            //13=口在视图中淡入淡出或滑动工具提示
            ::SystemParametersInfo(SPI_GETTOOLTIPANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 14:
            //14=口在鼠标指针下显示阴影
            ::SystemParametersInfo(SPI_GETCURSORSHADOW, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 15:
            //15=口在桌面上为图标标签使用阴影
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewShadow"));
            break;
        case 16:
            //16 = 口在最大化和最小化时显示窗口动画
            ANIMATIONINFO animation;
            animation.cbSize = sizeof(ANIMATIONINFO);
            ::SystemParametersInfo(SPI_GETANIMATION, sizeof(ANIMATIONINFO), &animation, 0);
            checked.append(std::to_string(animation.iMinAnimate));
            break;
        }
    }
}

void SetEffect(std::string radio, std::string checked)
{
    if (radio == "0") {
        // 让windows选择
        checked = "00000100000000000";//只开启字体平滑
    }
    else if (radio == "1") {
        // 最佳外观
        checked = "11111111111111111";
    }
    else if (radio == "2") {
        // 最佳性能
        checked = "00000000000000000";
    }
    else if (radio == "3") {
        // 接受外部自定义选项，判断值是否合法
        if (checked.length() != 17) {
            printf("自定义视觉效果值长度应该为17位长,请检查");
            return;
        }
    }
    BOOL bolValue;
    std::string strValue;
    for (size_t index = 0; index < checked.size();index++) {
        if (checked.at(index) != '0') {
            bolValue = TRUE;
            strValue = "1";
        }
        else {
            bolValue = FALSE;
            strValue = "0";
        }
        switch (index) {
        case 0:
            //0 = 口保存任务栏缩略图预览
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "AlwaysHibernateThumbnails", strValue, 4);
            break;
        case 1:
            //1=口窗口内的动画控件和元素
            ::SystemParametersInfo(SPI_SETCLIENTAREAANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 2:
            //2 = 口淡入淡出或滑动菜单到视图
             ::SystemParametersInfo(SPI_SETMENUANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 3:
            //3=口滑动打开组合框
            ::SystemParametersInfo(SPI_SETCOMBOBOXANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 4:
            //4=口平滑滚动列表框
            ::SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 5:
            //5=口平滑屏荨字体边缘
            ::SystemParametersInfo(SPI_SETFONTSMOOTHING, bolValue ? 1 : 0, NULL, SPIF_SENDCHANGE);
            SetRegValue("HKCU\\Control Panel\\Desktop", "FontSmoothing", bolValue ? "2":"0", 1);
            break;
        case 6:
            //6=口启用速览
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "EnableAeroPeek",strValue,4);
            break;
        case 7:
            //7=口任务栏中的动画
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "TaskbarAnimations",strValue,4);
            break;
        case 8:
            //8=口拖动时显示窗口内容
            ::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, bolValue, NULL, SPIF_SENDCHANGE);
            SetRegValue("HKCU\\Control Panel\\Desktop", "DragFullWindows", strValue, 1);
            break;
        case 9:
            //9=口显示缩略图，而不是显示图标:注册表项太多，暂定只有选择最佳性能时才勾选
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "IconsOnly", bolValue ? "0":"1", 4);
            break;
        case 10:
            //10=口显示亚透明的选择长方形
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewAlphaSelect", strValue,4);
            break;
        case 11:
            //11=口在窗口下显示阴影
            ::SystemParametersInfo(SPI_SETDROPSHADOW, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 12:
            //12 = 口在单击后淡出菜单
            ::SystemParametersInfo(SPI_SETSELECTIONFADE, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 13:
            //13=口在视图中淡入淡出或滑动工具提示
            ::SystemParametersInfo(SPI_SETTOOLTIPANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            ::SystemParametersInfo(SPI_SETTOOLTIPFADE, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 14:
            //14=口在鼠标指针下显示阴影
            ::SystemParametersInfo(SPI_SETCURSORSHADOW, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 15:
            //15=口在桌面上为图标标签使用阴影
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewShadow",strValue,4);
            break;
        case 16:
            //16 = 口在最大化和最小化时显示窗口动画
            ANIMATIONINFO animation;
            animation.cbSize = sizeof(ANIMATIONINFO);
            animation.iMinAnimate = bolValue ? 1:0;
            ::SystemParametersInfo(SPI_SETANIMATION, sizeof(ANIMATIONINFO), &animation, SPIF_SENDCHANGE);
            SetRegValue("HKCU\\Control Panel\\Desktop\\WindowsMetrics", "MinAnimate", strValue, 1);
            break;
        }
    }
    SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects", "VisualFXSetting", radio, 4);
}

void GetWallPaper(std::string& wallpaper, std::string& titleWallpaper, std::string& wallpaperStype)
{
    TCHAR wp[MAX_PATH];
    ::SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, wp, 0);
    int iLen = WideCharToMultiByte(CP_ACP, 0, wp, -1, NULL, 0, NULL, NULL);   //首先计算TCHAR 长度。

    char* chRtn = new char[iLen * sizeof(char)];  //定义一个 TCHAR 长度大小的 CHAR 类型。

    WideCharToMultiByte(CP_ACP, 0, wp, -1, chRtn, iLen, NULL, NULL);  //将TCHAR 类型的数据转换为 CHAR 类型。

    std::string str(chRtn); //最后将CHAR 类型数据 转换为 STRING 类型数据
    wallpaper = str;
    wallpaperStype = GetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle");
    titleWallpaper = GetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper");
}

//0：填充(Fill) 1：适应(Fit) 2：拉伸(Stretch) 3：平铺(Tile) 4：居中(Center) 5：跨区(Span)
void SetWallPaper(std::string wallpaper, int style)
{
    switch (style) {
    case 0:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle","10",1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper","0",1);
        break;
    case 1:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle", "6", 1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper", "0", 1);
        break;
    case 2:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle", "2", 1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper", "0", 1);
        break;
    case 3:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle", "0", 1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper", "1", 1);
        break;
    case 4:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle", "0", 1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper", "0", 1);
        break;
    case 5:
        SetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle", "22", 1);
        SetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper", "0", 1);
        break;
    default: 
        break;
    }
    ::SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (VOID*)s2ws(wallpaper).c_str(), 0);
    SetRegValue("HKCU\\Control Panel\\Desktop", "Wallpaper", wallpaper, 1);
}


void ShowSetting(std::string radio,std::string checked) {
    std::cout << "\n-----------------------------------------------------------------------------\n";
    std::cout << (radio == "0" ? " ●" : " ○") << " 让Windows选择计算机的最佳设置(0)\n";
    std::cout << (radio == "1" ? " ●" : " ○") << " 调整为最佳外观(1)\n";
    std::cout << (radio == "2" ? " ●" : " ○") << " 调整为最佳性能(2)\n";
    std::cout << (radio == "3" ? " ●" : " ○") << " 自定义(3)";
    std::cout << "\n-----------------------------------------------------------------------------\n";
    std::cout << " 序号|\t\t状态\n";
    std::cout << "-----|-----------------------------------------------------------------------\n";
    BOOL bolValue;
    for (size_t index = 0; index < checked.size(); index++) {
        if (checked.at(index) != '0') {
            bolValue = TRUE;
        }
        else {
            bolValue = FALSE;
        }
        switch (index) {
        case 0:
            std::cout << "  1  |\t" << (bolValue ? "√" : "口") << " 保存任务栏缩略图预览\n";
            break;
        case 1:
            std::cout << "  2  |\t" << (bolValue ? "√" : "口") << " 窗口内的动画控件和元素\n";
            break;
        case 2:
            //2 = 口淡入淡出或滑动菜单到视图
            std::cout << "  3  |\t" << (bolValue ? "√" : "口") << " 淡入淡出或滑动菜单到视图\n";
            break;
        case 3:
            //3=口滑动打开组合框 
            std::cout << "  4  |\t" << (bolValue ? "√" : "口") << " 滑动打开组合框\n";
            break;
        case 4:
            //4=口平滑滚动列表框
            std::cout << "  5  |\t" << (bolValue ? "√" : "口") << " 平滑滚动列表框\n";
            break;
        case 5:
            //5=口平滑屏荨字体边缘
            std::cout << "  6  |\t" << (bolValue ? "√" : "口") << " 平滑屏荨字体边缘\n";
            break;
        case 6:
            //6=口启用速览
            std::cout << "  7  |\t" << (bolValue ? "√" : "口") << " 启用速览\n";
            break;
        case 7:
            //7=口任务栏中的动画
            std::cout << "  8  |\t" << (bolValue ? "√" : "口") << " 任务栏中的动画\n";
            break;
        case 8:
            //8=口拖动时显示窗口内容
            std::cout << "  9  |\t" << (bolValue ? "√" : "口") << " 拖动时显示窗口内容\n";
            break;
        case 9:
            //9=口显示缩略图，而不是显示图标:注册表项太多，暂定只有选择最佳性能时才勾选
            std::cout << " 10  |\t" << (bolValue ? "√" : "口") << " 显示缩略图，而不是显示图标\n";
            break;
        case 10:
            //10=口显示亚透明的选择长方形
            std::cout << " 11  |\t" << (bolValue ? "√" : "口") << " 显示亚透明的选择长方形\n";
            break;
        case 11:
            //11=口在窗口下显示阴影
            std::cout << " 12  |\t" << (bolValue ? "√" : "口") << " 在窗口下显示阴影\n";
            break;
        case 12:
            //12 = 口在单击后淡出菜单
            std::cout << " 13  |\t" << (bolValue ? "√" : "口") << " 在单击后淡出菜单\n";
            break;
        case 13:
            //13=口在视图中淡入淡出或滑动工具提示
            std::cout << " 14  |\t" << (bolValue ? "√" : "口") << " 在视图中淡入淡出或滑动工具提示\n";
            break;
        case 14:
            //14=口在鼠标指针下显示阴影
            std::cout << " 15  |\t" << (bolValue ? "√" : "口") << " 在鼠标指针下显示阴影\n";
            break;
        case 15:
            //15=口在桌面上为图标标签使用阴影
            std::cout << " 16  |\t" << (bolValue ? "√" : "口") << " 在桌面上为图标标签使用阴影\n";
            break;
        case 16:
            //16 = 口在最大化和最小化时显示窗口动画
            std::cout << " 17  |\t" << (bolValue ? "√" : "口") << " 在最大化和最小化时显示窗口动画\n";
            break;
        }
    }
    std::cout << "-----------------------------------------------------------------------------\n";
}