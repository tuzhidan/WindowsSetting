#include <windows.h>

/*
int in_out=0 - �����
int in_out=1 - ������
*/
void CreateFireWallPolicy(char* in_out, LPCTSTR ruleName, LPCTSTR appPath, LPCTSTR ruleDesc, LPCTSTR ruleGroup);
void DeleteFireWallPolicy(LPCTSTR ruleName);
/*
	open=0 - ������
	open=1 - ����

	deny=0 - ����
	deny=1 - ������
*/
void UpdateFireWallPolicy(LPCTSTR ruleName,int open,int deny);