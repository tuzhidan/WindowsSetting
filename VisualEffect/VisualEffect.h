#pragma once

#include <String>

// ��ȡ��ǰ���õ��Ӿ�Ч��
void GetEffect(std::string &radio, std::string &checked);
/*
	�����Ӿ�Ч��
	radio:��ѡֵ��0=��windowsѡ�� 1=������ 2=������� 3=�Զ���
	checked:��ѡֵ��
					0=�ڱ�������������ͼԤ��
					1=�ڴ����ڵĶ����ؼ���Ԫ��
					2=�ڵ��뵭���򻬶��˵�����ͼ
					3=�ڻ�������Ͽ�
					4=��ƽ�������б��
					5=��ƽ����ݡ�����Ե
					6=����������
					7=���������еĶ���
					8=���϶�ʱ��ʾ��������
					9=����ʾ����ͼ����������ʾͼ��
					10=����ʾ��͸����ѡ�񳤷���
					11=���ڴ�������ʾ��Ӱ
					12=���ڵ����󵭳��˵�
					13=������ͼ�е��뵭���򻬶�������ʾ
					14=�������ָ������ʾ��Ӱ
					15=����������Ϊͼ���ǩʹ����Ӱ
					16=������󻯺���С��ʱ��ʾ���ڶ���
*/
void SetEffect(std::string radio, std::string checked);

void GetWallPaper(std::string& wallpaper, std::string& titleWallpaper, std::string& wallpaperStype);

/*
	style ��ֽ���϶�
	0����� 1����Ӧ 2������ 3��ƽ�� 4������ 5������
*/
void SetWallPaper(std::string wallpaper, int style = 0);

void ShowSetting(std::string radio,std::string checked);