#pragma once


#include <string>

std::string ws2s(const std::wstring& ws);
std::wstring s2ws(const std::string& s);
int is_begin_with(const char* str1, char* str2);
int is_end_with(const char* str1, char* str2);