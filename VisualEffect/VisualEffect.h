#pragma once

#include <String>

// 获取当前设置的视觉效果
void GetEffect(std::string &radio, std::string &checked);
/*
	设置视觉效果
	radio:单选值，0=让windows选择 1=最佳外观 2=最佳性能 3=自定义
	checked:多选值：
					0=口保存任务栏缩略图预览
					1=口窗口内的动画控件和元素
					2=口淡入淡出或滑动菜单到视图
					3=口滑动打开组合框
					4=口平滑滚动列表框
					5=口平滑屏荨字体边缘
					6=口启用速览
					7=口任务栏中的动画
					8=口拖动时显示窗口内容
					9=口显示缩略图，而不是显示图标
					10=口显示亚透明的选择长方形
					11=口在窗口下显示阴影
					12=口在单击后淡出菜单
					13=口在视图中淡入淡出或滑动工具提示
					14=口在鼠标指针下显示阴影
					15=口在桌面上为图标标签使用阴影
					16=口在最大化和最小化时显示窗口动画
*/
void SetEffect(std::string radio, std::string checked);

void GetWallPaper(std::string& wallpaper, std::string& titleWallpaper, std::string& wallpaperStype);

/*
	style 壁纸契合度
	0：填充 1：适应 2：拉伸 3：平铺 4：居中 5：跨区
*/
void SetWallPaper(std::string wallpaper, int style = 0);

void ShowSetting(std::string radio,std::string checked);