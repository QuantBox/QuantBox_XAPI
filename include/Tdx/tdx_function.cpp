#include "stdafx.h"
#include <stdio.h>

#include "tdx_function.h"
#include "tdx_enum.h"

#ifdef TDXAPI_EXPORTS

/*
注意：以下代码只是几个数据表的读写方式，当做示例使用
此cpp请不要添加到项目中，而是只添加h文件
因为由哪里生成的内存块就由哪里删除，由dll生成的内存，由dll中的函数来删
如果把这个函数编译到exe中，会导致是由外部的exe中函数来删除了，会出错

*/

void PrintTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		char buf[512] = { 0 };
		printf("%d_%s|",//"%d,%s,%d,%d,%d,%d,%d",
			pRow->FieldID, pRow->FieldName, pRow->a, pRow->b, pRow->Len, pRow->d, pRow->e);

		++i;
		pRow = ppHeader[i];
	}
	printf("\n");
}

FieldInfo_STRUCT** CopyTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return nullptr;

	int count = GetCountTableHeader(ppHeader)+1;
	FieldInfo_STRUCT** ppNew = new FieldInfo_STRUCT*[count];
	ppNew[count - 1] = nullptr;
	
	for (int i = 0; i < count;++i)
	{
		//ppNew[i] = ppHeader[i];
		ppNew[i] = new FieldInfo_STRUCT;
		memcpy(ppNew[i], ppHeader[i], sizeof(FieldInfo_STRUCT));
	}
	return ppNew;
}

void DeleteTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return;

	//原有创建方法是一个连续区域，使用时容易出错
	//delete[] ppHeader;

	int i = 0;
	while (ppHeader[i] != 0)
	{
		delete[] ppHeader[i];

		++i;
	}

	delete[] ppHeader;
}

int GetIndexByFieldName(FieldInfo_STRUCT** ppHeader, char* FieldName)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		if (strcmp(pRow->FieldName,FieldName)==0)
		{
			return i;
		}

		++i;
		pRow = ppHeader[i];
	}
	return -1;
}

int GetIndexByFieldID(FieldInfo_STRUCT** ppHeader, int FieldID)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		if (pRow->FieldID == FieldID)
		{
			return i;
		}

		++i;
		pRow = ppHeader[i];
	}
	return -1;
}

int GetCountTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		++i;
		pRow = ppHeader[i];
	}
	return i;
}

void PrintTableBody(char** ppTable)
{
	if (ppTable == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		printf("%d:",i);
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				printf("%s|", p);
			}
			else
				break;
		}
		printf("\n");
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return;
}

void PrintTableBody(char** ppTable, int count)
{
	if (ppTable == nullptr)
		return;

	for (int i = 0; i < count;++i)
	{
		printf("%d:", i);
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				printf("%s|", p);
				
			}
			else
				break;
		}
		printf("\n");
	}

	return;
}

// 得到某一行某一列
char* GetAtTableBody(char** ppTable, int row, int col)
{
	if (ppTable == nullptr)
		return nullptr;

	if (col >= COL_EACH_ROW)
		return nullptr;

	if (row >= GetRowCountTableBody(ppTable))
	{
		return nullptr;
	}

	return ppTable[row * COL_EACH_ROW + col];
}

int GetRowCountTableBody(char** ppTable)
{
	if (ppTable == nullptr)
		return -1;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return i;
}

void DeleteTableBody(char** ppTable)
{
	// 要注意的是什么时候停止删除
	if (ppTable == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			delete[] ppTable[i * COL_EACH_ROW + j];
		}
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	delete[] ppTable;

	return;
}

void DeleteTableBody(char** ppTable, int count)
{
	if (ppTable == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				delete[] ppTable[i * COL_EACH_ROW + j];
			}
			else
				break;
		}
	}

	delete[] ppTable;
}

void PrintError(Error_STRUCT* pErr)
{
	if (pErr == nullptr)
	{
		return;
	}

	printf("%d,%d,%s\n", pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
}

void PrintErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return;

	int i = 0;
	Error_STRUCT* pErr = pErrs[i];
	while (pErr != nullptr)
	{
		printf("%d:%d,%d,%s\n",i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);

		++i;
		pErr = pErrs[i];
	}
}

void PrintErrors(Error_STRUCT** pErrs, int count)
{
	if (pErrs == nullptr)
	{
		return;
	}

	for (int i = 0; i < count;++i)
	{
		Error_STRUCT* pErr = pErrs[i];
		if (pErr)
		{
			printf("%d:%d,%d,%s\n", i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
		}
	}
}

void DeleteError(Error_STRUCT* pErr)
{
	delete[] pErr;
}

void DeleteErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return;

	int i = 0;
	while (pErrs[i] != 0)
	{
		delete[] pErrs[i];

		++i;
	}

	delete[] pErrs;
}

void DeleteErrors(Error_STRUCT** pErrs, int count)
{
	if (pErrs == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		delete[] pErrs[i];
	}

	delete[] pErrs;
}

int GetCountErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return -1;

	int i = 0;
	Error_STRUCT* pRow = pErrs[i];
	while (pRow != 0)
	{
		++i;
		pRow = pErrs[i];
	}
	return i;
}

#else
#endif