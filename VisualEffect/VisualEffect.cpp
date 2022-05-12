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
            //0 = �ڱ�������������ͼԤ��
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "AlwaysHibernateThumbnails"));
            break;
        case 1:
            //1=�ڴ����ڵĶ����ؼ���Ԫ��
            ::SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 2:
            //2 = �ڵ��뵭���򻬶��˵�����ͼ
            ::SystemParametersInfo(SPI_GETMENUANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 3:
            //3=�ڻ�������Ͽ�
            ::SystemParametersInfo(SPI_GETCOMBOBOXANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 4:
            //4=��ƽ�������б��
            ::SystemParametersInfo(SPI_GETLISTBOXSMOOTHSCROLLING, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 5:
            //5=��ƽ����ݡ�����Ե
            ::SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &bolValue, 0);
            checked.append(std::to_string(bolValue));
            break;
        case 6:
            //6=����������
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "EnableAeroPeek"));
            break;
        case 7:
            //7=���������еĶ���
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "TaskbarAnimations"));
            break;
        case 8:
            //8=���϶�ʱ��ʾ��������
            ::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bolValue, 0);
            checked.append(std::to_string(bolValue));
            break;
        case 9:
            //9=����ʾ����ͼ����������ʾͼ��:ע�����̫�࣬�ݶ�ֻ��ѡ���������ʱ�Ź�ѡ
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "IconsOnly") == "1" ? "0" : "1");
            break;
        case 10:
            //10=����ʾ��͸����ѡ�񳤷���
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewAlphaSelect"));
            break;
        case 11:
            //11=���ڴ�������ʾ��Ӱ
            ::SystemParametersInfo(SPI_GETDROPSHADOW, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 12:
            //12 = ���ڵ����󵭳��˵�
            ::SystemParametersInfo(SPI_GETSELECTIONFADE, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 13:
            //13=������ͼ�е��뵭���򻬶�������ʾ
            ::SystemParametersInfo(SPI_GETTOOLTIPANIMATION, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 14:
            //14=�������ָ������ʾ��Ӱ
            ::SystemParametersInfo(SPI_GETCURSORSHADOW, NULL, &bolValue, 0);
            checked.append(bolValue ? "1" : "0");
            break;
        case 15:
            //15=����������Ϊͼ���ǩʹ����Ӱ
            checked.append(GetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewShadow"));
            break;
        case 16:
            //16 = ������󻯺���С��ʱ��ʾ���ڶ���
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
        // ��windowsѡ��
        checked = "00000100000000000";//ֻ��������ƽ��
    }
    else if (radio == "1") {
        // ������
        checked = "11111111111111111";
    }
    else if (radio == "2") {
        // �������
        checked = "00000000000000000";
    }
    else if (radio == "3") {
        // �����ⲿ�Զ���ѡ��ж�ֵ�Ƿ�Ϸ�
        if (checked.length() != 17) {
            printf("�Զ����Ӿ�Ч��ֵ����Ӧ��Ϊ17λ��,����");
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
            //0 = �ڱ�������������ͼԤ��
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "AlwaysHibernateThumbnails", strValue, 4);
            break;
        case 1:
            //1=�ڴ����ڵĶ����ؼ���Ԫ��
            ::SystemParametersInfo(SPI_SETCLIENTAREAANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 2:
            //2 = �ڵ��뵭���򻬶��˵�����ͼ
             ::SystemParametersInfo(SPI_SETMENUANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 3:
            //3=�ڻ�������Ͽ�
            ::SystemParametersInfo(SPI_SETCOMBOBOXANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 4:
            //4=��ƽ�������б��
            ::SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 5:
            //5=��ƽ����ݡ�����Ե
            ::SystemParametersInfo(SPI_SETFONTSMOOTHING, bolValue ? 1 : 0, NULL, SPIF_SENDCHANGE);
            SetRegValue("HKCU\\Control Panel\\Desktop", "FontSmoothing", bolValue ? "2":"0", 1);
            break;
        case 6:
            //6=����������
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\DWM", "EnableAeroPeek",strValue,4);
            break;
        case 7:
            //7=���������еĶ���
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "TaskbarAnimations",strValue,4);
            break;
        case 8:
            //8=���϶�ʱ��ʾ��������
            ::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, bolValue, NULL, SPIF_SENDCHANGE);
            SetRegValue("HKCU\\Control Panel\\Desktop", "DragFullWindows", strValue, 1);
            break;
        case 9:
            //9=����ʾ����ͼ����������ʾͼ��:ע�����̫�࣬�ݶ�ֻ��ѡ���������ʱ�Ź�ѡ
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "IconsOnly", bolValue ? "0":"1", 4);
            break;
        case 10:
            //10=����ʾ��͸����ѡ�񳤷���
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewAlphaSelect", strValue,4);
            break;
        case 11:
            //11=���ڴ�������ʾ��Ӱ
            ::SystemParametersInfo(SPI_SETDROPSHADOW, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 12:
            //12 = ���ڵ����󵭳��˵�
            ::SystemParametersInfo(SPI_SETSELECTIONFADE, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 13:
            //13=������ͼ�е��뵭���򻬶�������ʾ
            ::SystemParametersInfo(SPI_SETTOOLTIPANIMATION, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            ::SystemParametersInfo(SPI_SETTOOLTIPFADE, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 14:
            //14=�������ָ������ʾ��Ӱ
            ::SystemParametersInfo(SPI_SETCURSORSHADOW, NULL, (VOID*)bolValue, SPIF_SENDCHANGE);
            break;
        case 15:
            //15=����������Ϊͼ���ǩʹ����Ӱ
            SetRegValue("HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "ListviewShadow",strValue,4);
            break;
        case 16:
            //16 = ������󻯺���С��ʱ��ʾ���ڶ���
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
    int iLen = WideCharToMultiByte(CP_ACP, 0, wp, -1, NULL, 0, NULL, NULL);   //���ȼ���TCHAR ���ȡ�

    char* chRtn = new char[iLen * sizeof(char)];  //����һ�� TCHAR ���ȴ�С�� CHAR ���͡�

    WideCharToMultiByte(CP_ACP, 0, wp, -1, chRtn, iLen, NULL, NULL);  //��TCHAR ���͵�����ת��Ϊ CHAR ���͡�

    std::string str(chRtn); //���CHAR �������� ת��Ϊ STRING ��������
    wallpaper = str;
    wallpaperStype = GetRegValue("HKCU\\Control Panel\\Desktop", "WallpaperStyle");
    titleWallpaper = GetRegValue("HKCU\\Control Panel\\Desktop", "TileWallpaper");
}

//0�����(Fill) 1����Ӧ(Fit) 2������(Stretch) 3��ƽ��(Tile) 4������(Center) 5������(Span)
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
    std::cout << (radio == "0" ? " ��" : " ��") << " ��Windowsѡ���������������(0)\n";
    std::cout << (radio == "1" ? " ��" : " ��") << " ����Ϊ������(1)\n";
    std::cout << (radio == "2" ? " ��" : " ��") << " ����Ϊ�������(2)\n";
    std::cout << (radio == "3" ? " ��" : " ��") << " �Զ���(3)";
    std::cout << "\n-----------------------------------------------------------------------------\n";
    std::cout << " ���|\t\t״̬\n";
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
            std::cout << "  1  |\t" << (bolValue ? "��" : "��") << " ��������������ͼԤ��\n";
            break;
        case 1:
            std::cout << "  2  |\t" << (bolValue ? "��" : "��") << " �����ڵĶ����ؼ���Ԫ��\n";
            break;
        case 2:
            //2 = �ڵ��뵭���򻬶��˵�����ͼ
            std::cout << "  3  |\t" << (bolValue ? "��" : "��") << " ���뵭���򻬶��˵�����ͼ\n";
            break;
        case 3:
            //3=�ڻ�������Ͽ� 
            std::cout << "  4  |\t" << (bolValue ? "��" : "��") << " ��������Ͽ�\n";
            break;
        case 4:
            //4=��ƽ�������б��
            std::cout << "  5  |\t" << (bolValue ? "��" : "��") << " ƽ�������б��\n";
            break;
        case 5:
            //5=��ƽ����ݡ�����Ե
            std::cout << "  6  |\t" << (bolValue ? "��" : "��") << " ƽ����ݡ�����Ե\n";
            break;
        case 6:
            //6=����������
            std::cout << "  7  |\t" << (bolValue ? "��" : "��") << " ��������\n";
            break;
        case 7:
            //7=���������еĶ���
            std::cout << "  8  |\t" << (bolValue ? "��" : "��") << " �������еĶ���\n";
            break;
        case 8:
            //8=���϶�ʱ��ʾ��������
            std::cout << "  9  |\t" << (bolValue ? "��" : "��") << " �϶�ʱ��ʾ��������\n";
            break;
        case 9:
            //9=����ʾ����ͼ����������ʾͼ��:ע�����̫�࣬�ݶ�ֻ��ѡ���������ʱ�Ź�ѡ
            std::cout << " 10  |\t" << (bolValue ? "��" : "��") << " ��ʾ����ͼ����������ʾͼ��\n";
            break;
        case 10:
            //10=����ʾ��͸����ѡ�񳤷���
            std::cout << " 11  |\t" << (bolValue ? "��" : "��") << " ��ʾ��͸����ѡ�񳤷���\n";
            break;
        case 11:
            //11=���ڴ�������ʾ��Ӱ
            std::cout << " 12  |\t" << (bolValue ? "��" : "��") << " �ڴ�������ʾ��Ӱ\n";
            break;
        case 12:
            //12 = ���ڵ����󵭳��˵�
            std::cout << " 13  |\t" << (bolValue ? "��" : "��") << " �ڵ����󵭳��˵�\n";
            break;
        case 13:
            //13=������ͼ�е��뵭���򻬶�������ʾ
            std::cout << " 14  |\t" << (bolValue ? "��" : "��") << " ����ͼ�е��뵭���򻬶�������ʾ\n";
            break;
        case 14:
            //14=�������ָ������ʾ��Ӱ
            std::cout << " 15  |\t" << (bolValue ? "��" : "��") << " �����ָ������ʾ��Ӱ\n";
            break;
        case 15:
            //15=����������Ϊͼ���ǩʹ����Ӱ
            std::cout << " 16  |\t" << (bolValue ? "��" : "��") << " ��������Ϊͼ���ǩʹ����Ӱ\n";
            break;
        case 16:
            //16 = ������󻯺���С��ʱ��ʾ���ڶ���
            std::cout << " 17  |\t" << (bolValue ? "��" : "��") << " ����󻯺���С��ʱ��ʾ���ڶ���\n";
            break;
        }
    }
    std::cout << "-----------------------------------------------------------------------------\n";
}