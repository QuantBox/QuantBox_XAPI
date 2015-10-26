#include "stdafx.h"
#include "IDGenerator.h"

CIDGenerator::CIDGenerator()
{
	m_id = 0;
	memset(m_IDString, 0, sizeof(OrderIDType));
	memset(m_Prefix, 0, sizeof(OrderIDType));
}

CIDGenerator::~CIDGenerator()
{
}

void CIDGenerator::SetPrefix(const char* prefix)
{
	strncpy(m_Prefix, prefix, 32);
}

long CIDGenerator::GetID()
{
	return m_id++;
}

const char* CIDGenerator::GetIDString()
{
	sprintf(m_IDString, "%s:%d", m_Prefix, m_id++);
	return m_IDString;
}