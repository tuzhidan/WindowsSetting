#include <windows.h>

/*
int in_out=0 - 入规则
int in_out=1 - 出规则
*/
void CreateFireWallPolicy(char* in_out, LPCTSTR ruleName, LPCTSTR appPath, LPCTSTR ruleDesc, LPCTSTR ruleGroup);
void DeleteFireWallPolicy(LPCTSTR ruleName);
/*
	open=0 - 不启用
	open=1 - 启用

	deny=0 - 允许
	deny=1 - 不允许
*/
void UpdateFireWallPolicy(LPCTSTR ruleName,int open,int deny);