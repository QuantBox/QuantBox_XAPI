#ifndef _TOOLKIT_H_
#define _TOOLKIT_H_

#include <vector>
#include <set>
#include <string>

using namespace std;

void WriteLog(const char *fmt, ...);

//输入路径，生成多级目录
void makedirs(const char* dir);

//将字符串按指定字符分割
typedef void fnGetSetFromStringProcess(char* token);
char* GetSetFromString(const char* szString, const char* seps, vector<char*>& vct, set<char*>& st, int modify, set<string>& st2, int before = 1, const char* prefix = nullptr);

int GetUpdateTime(char* UpdateTime, int* _UpdateTime, int* UpdateMillisec);
void GetExchangeTime_CZCE(int iTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec);
void GetExchangeTime_Undefined(int iTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec);
void GetExchangeTime_DCE(char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec);
void GetExchangeTime(char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec);
void GetExchangeTime(time_t Time, int* _TradingDay, int* _ActionDay, int* _UpdateTime);

int GetDate(char* TradingDay);
int GetTime(char* UpdateTime);

double my_round(float val, int x = 0);

//根据OnFrontDisconnected(int nReason)的值填上错误消息
void GetOnFrontDisconnectedMsg(int ErrorId, char* ErrorMsg);

#endif