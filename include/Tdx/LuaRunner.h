#if !defined(_LUA_RUNNER_H)
#define _LUA_RUNNER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef TDXAPI_EXPORTS
#define TDXAPI_API __declspec(dllexport)
#else
#define TDXAPI_API __declspec(dllimport)
#endif

#include "tdx_struct.h"

class TDXAPI_API CLuaRunner
{
public:
	// 创建配置文加载器
	static CLuaRunner* CreateRunner();
	virtual void Release()=0;

	// 通过Lua文件登录
	//		TdxPath*:通达信目录，目录必需以“\\”结尾
	//		LuaFileOrString:登录用的lua配置文件或文件中内容
	//		bFileOrString:指明第二个参数是文件路径还是字符串内容
	//		bEncrypted:指明内容是否加密
	//		szAccount:账号
	//		szPassword:密码
	//		pppResults:结果集
	//		ppErr:错误信息
	//virtual void Login(const char* TdxPath, const char* LuaFileOrString, bool bFileOrString, bool bEncrypted, const char* szAccount, const char* szPassword, char*** pppResults, Error_STRUCT** ppErr) = 0;

	//virtual const char* GetAccount() = 0;
	//virtual const char* GetPassword() = 0;
	//virtual void* GetClient() = 0;

	virtual void LoadScript(const char* LuaFileOrString, bool bFileOrString, bool bEncrypted) = 0;

	virtual void Init(const char* TdxPath, Error_STRUCT** ppErr) = 0;
	virtual void Exit() = 0;

	virtual void* Login(const char* szAccount, const char* szPassword, char*** pppResults, Error_STRUCT** ppErr) = 0;
	virtual void Logout(void* client) = 0;

protected:
	CLuaRunner();
	~CLuaRunner(){};
};

#endif

