#pragma once

#include <atomic>

#include "ApiStruct.h"

using namespace std;

class CIDGenerator
{
public:
	CIDGenerator();
	~CIDGenerator();

	// 不能太长了，需要自动截断，目前计划不能超过32
	void SetPrefix(const char* prefix);

	const char* GetIDString();
	long GetID();
private:
	atomic<long> m_id;
	OrderIDType m_IDString;
	OrderIDType m_Prefix;
};

