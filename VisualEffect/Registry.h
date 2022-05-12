#pragma once
#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#include <string>

std::string GetRegValue(const std::string& strUrl, const std::string& strKey);
int SetRegValue(const std::string& strUrl, const std::string& strKey, const std::string& strValue, int valueType);
int CreateRegKey(const std::string& strUrl);

#endif  //__REGISTRY_H__