#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#include "tdx_function.h"
#include "tdx_enum.h"
#include "../TdxApi/TcSdk_Const.h"

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

void OutputCSVTableHeader(FILE* pFile, FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr || pFile == nullptr)
		return;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		char buf[512] = { 0 };
		fprintf(pFile,"%d_%s,",//"%d,%s,%d,%d,%d,%d,%d",
			pRow->FieldID, pRow->FieldName, pRow->a, pRow->b, pRow->Len, pRow->d, pRow->e);

		++i;
		pRow = ppHeader[i];
	}
	fprintf(pFile, "\n");
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

void OutputCSVTableBody(FILE* pFile, char** ppTable)
{
	if (ppTable == nullptr || pFile == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		//printf("%d:", i);
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				fprintf(pFile, "%s,", p);
			}
			else
				break;
		}
		fprintf(pFile, "\n");
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return;
}

void OutputCSVTableBody(FILE* pFile, char** ppTable, int count)
{
	if (ppTable == nullptr || pFile == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		//printf("%d:", i);
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				fprintf(pFile, "%s,", p);

			}
			else
				break;
		}
		fprintf(pFile, "\n");
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


void CharTable2GDLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GDLB_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	GDLB_STRUCT** ppResults = new GDLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_123 = GetIndexByFieldID(ppFieldInfos, FIELD_GDDM);
	int col_124 = GetIndexByFieldID(ppFieldInfos, FIELD_GDMC);
	int col_121 = GetIndexByFieldID(ppFieldInfos, FIELD_ZJZH);
	int col_125 = GetIndexByFieldID(ppFieldInfos, FIELD_ZHLB);
	int col_281 = GetIndexByFieldID(ppFieldInfos, FIELD_RZRQBS);
	int col_1213 = GetIndexByFieldID(ppFieldInfos, FIELD_BLXX);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GDLB_STRUCT();

		//if (col_123 >= 0)
		strcpy(ppResults[i]->GDDM, ppTable[i * COL_EACH_ROW + col_123]);
		//if (col_124 >= 0)
		strcpy(ppResults[i]->GDMC, ppTable[i * COL_EACH_ROW + col_124]);
		if (col_121 >= 0)
			strcpy(ppResults[i]->ZJZH, ppTable[i * COL_EACH_ROW + col_121]);
		//if (col_125 >= 0)
		strcpy(ppResults[i]->ZHLB, ppTable[i * COL_EACH_ROW + col_125]);
		if (col_281 >= 0)
			strcpy(ppResults[i]->RZRQBS, ppTable[i * COL_EACH_ROW + col_281]);
		//if (col_1213 >= 0)
		//	strcpy(ppResults[i]->BLXX, ppTable[i * COL_EACH_ROW + col_1213]);

		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->RZRQBS_ = atoi(ppResults[i]->RZRQBS);
	}
}

void CharTable2WTLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, WTLB_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	WTLB_STRUCT** ppResults = new WTLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_142 = GetIndexByFieldID(ppFieldInfos, FIELD_WTRQ);
	int col_143 = GetIndexByFieldID(ppFieldInfos, FIELD_WTSJ);
	int col_123 = GetIndexByFieldID(ppFieldInfos, FIELD_GDDM);
	int col_140 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQDM);
	int col_141 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQMC);
	int col_130 = GetIndexByFieldID(ppFieldInfos, FIELD_MMBZ);
	int col_131 = GetIndexByFieldID(ppFieldInfos, FIELD_WTLB);
	int col_100 = GetIndexByFieldID(ppFieldInfos, FIELD_JYSDM);
	int col_145 = GetIndexByFieldID(ppFieldInfos, FIELD_WTJG);
	int col_144 = GetIndexByFieldID(ppFieldInfos, FIELD_WTSL);
	int col_153 = GetIndexByFieldID(ppFieldInfos, FIELD_CJJG);
	int col_152 = GetIndexByFieldID(ppFieldInfos, FIELD_CJSL);
	int col_162 = GetIndexByFieldID(ppFieldInfos, FIELD_CDSL);
	int col_146 = GetIndexByFieldID(ppFieldInfos, FIELD_WTBH);
	int col_194 = GetIndexByFieldID(ppFieldInfos, FIELD_BJFS);
	int col_147 = GetIndexByFieldID(ppFieldInfos, FIELD_ZTSM);
	int col_161 = GetIndexByFieldID(ppFieldInfos, FIELD_DJZJ);
	int col_1213 = GetIndexByFieldID(ppFieldInfos, FIELD_BLXX);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new WTLB_STRUCT();

		if (col_142 >= 0)
			strcpy(ppResults[i]->WTRQ, ppTable[i * COL_EACH_ROW + col_142]);
		if (col_143 >= 0)
			strcpy(ppResults[i]->WTSJ, ppTable[i * COL_EACH_ROW + col_143]);
		//if (col_123 >= 0)
		strcpy(ppResults[i]->GDDM, ppTable[i * COL_EACH_ROW + col_123]);
		//if (col_140 >= 0)
		strcpy(ppResults[i]->ZQDM, ppTable[i * COL_EACH_ROW + col_140]);
		//if (col_141 >= 0)
		strcpy(ppResults[i]->ZQMC, ppTable[i * COL_EACH_ROW + col_141]);
		//if (col_130 >= 0)
		strcpy(ppResults[i]->MMBZ, ppTable[i * COL_EACH_ROW + col_130]);
		//if (col_131 >= 0)
		strcpy(ppResults[i]->WTLB, ppTable[i * COL_EACH_ROW + col_131]);
		//if (col_100 >= 0)
		strcpy(ppResults[i]->JYSDM, ppTable[i * COL_EACH_ROW + col_100]);
		//if (col_145 >= 0)
		strcpy(ppResults[i]->WTJG, ppTable[i * COL_EACH_ROW + col_145]);
		//if (col_144 >= 0)
		strcpy(ppResults[i]->WTSL, ppTable[i * COL_EACH_ROW + col_144]);
		//if (col_153 >= 0)
		strcpy(ppResults[i]->CJJG, ppTable[i * COL_EACH_ROW + col_153]);
		//if (col_152 >= 0)
		strcpy(ppResults[i]->CJSL, ppTable[i * COL_EACH_ROW + col_152]);
		//if (col_162 >= 0)
		strcpy(ppResults[i]->CDSL, ppTable[i * COL_EACH_ROW + col_162]);
		//if (col_146 >= 0)
		strcpy(ppResults[i]->WTBH, ppTable[i * COL_EACH_ROW + col_146]);
		//if (col_194 >= 0)
		strcpy(ppResults[i]->BJFS, ppTable[i * COL_EACH_ROW + col_194]);
		if (col_147 >= 0)
			strcpy(ppResults[i]->ZTSM, ppTable[i * COL_EACH_ROW + col_147]);
		if (col_161 >= 0)
			strcpy(ppResults[i]->DJZJ, ppTable[i * COL_EACH_ROW + col_161]);
		//if (col_1213 >= 0)
		//	strcpy(ppResults[i]->BLXX, ppTable[i * COL_EACH_ROW + col_1213]);

		ppResults[i]->WTRQ_ = atoi(ppResults[i]->WTRQ);
		ppResults[i]->MMBZ_ = atoi(ppResults[i]->MMBZ);
		ppResults[i]->JYSDM_ = atoi(ppResults[i]->JYSDM);
		ppResults[i]->WTJG_ = atof(ppResults[i]->WTJG);
		ppResults[i]->WTSL_ = atoi(ppResults[i]->WTSL);
		ppResults[i]->CJJG_ = atof(ppResults[i]->CJJG);
		ppResults[i]->CJSL_ = atoi(ppResults[i]->CJSL);
		ppResults[i]->CDSL_ = atoi(ppResults[i]->CDSL);
		ppResults[i]->DJZJ_ = atof(ppResults[i]->DJZJ);

		int HH = 0, mm = 0, ss = 0;
		GetUpdateTime_HH_mm_ss(ppResults[i]->WTSJ, &HH, &mm, &ss);
		ppResults[i]->WTSJ_ = HH * 10000 + mm * 100 + ss;
	}
}

void CharTable2CJLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, CJLB_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	CJLB_STRUCT** ppResults = new CJLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_150 = GetIndexByFieldID(ppFieldInfos, FIELD_CJRQ);
	int col_151 = GetIndexByFieldID(ppFieldInfos, FIELD_CJSJ);
	int col_123 = GetIndexByFieldID(ppFieldInfos, FIELD_GDDM);
	int col_140 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQDM);
	int col_141 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQMC);
	int col_130 = GetIndexByFieldID(ppFieldInfos, FIELD_MMBZ);
	int col_131 = GetIndexByFieldID(ppFieldInfos, FIELD_WTLB);
	int col_153 = GetIndexByFieldID(ppFieldInfos, FIELD_CJJG);
	int col_152 = GetIndexByFieldID(ppFieldInfos, FIELD_CJSL);
	int col_303 = GetIndexByFieldID(ppFieldInfos, FIELD_FSJE);
	int col_304 = GetIndexByFieldID(ppFieldInfos, FIELD_SYJE);
	int col_206 = GetIndexByFieldID(ppFieldInfos, FIELD_YJ);
	int col_210 = GetIndexByFieldID(ppFieldInfos, FIELD_YHS);
	int col_207 = GetIndexByFieldID(ppFieldInfos, FIELD_GHF);
	int col_208 = GetIndexByFieldID(ppFieldInfos, FIELD_CJF);
	int col_155 = GetIndexByFieldID(ppFieldInfos, FIELD_CJBH);
	int col_167 = GetIndexByFieldID(ppFieldInfos, FIELD_CDBZ);
	int col_146 = GetIndexByFieldID(ppFieldInfos, FIELD_WTBH);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new CJLB_STRUCT();

		if (col_150 >= 0)
			strcpy(ppResults[i]->CJRQ, ppTable[i * COL_EACH_ROW + col_150]);
		//if (col_151 >= 0)
		strcpy(ppResults[i]->CJSJ, ppTable[i * COL_EACH_ROW + col_151]);
		//if (col_123 >= 0)
		strcpy(ppResults[i]->GDDM, ppTable[i * COL_EACH_ROW + col_123]);
		//if (col_140 >= 0)
		strcpy(ppResults[i]->ZQDM, ppTable[i * COL_EACH_ROW + col_140]);
		//if (col_141 >= 0)
		strcpy(ppResults[i]->ZQMC, ppTable[i * COL_EACH_ROW + col_141]);
		//if (col_130 >= 0)
		strcpy(ppResults[i]->MMBZ, ppTable[i * COL_EACH_ROW + col_130]);
		//if (col_131 >= 0)
		strcpy(ppResults[i]->WTLB, ppTable[i * COL_EACH_ROW + col_131]);
		//if (col_153 >= 0)
		strcpy(ppResults[i]->CJJG, ppTable[i * COL_EACH_ROW + col_153]);
		//if (col_152 >= 0)
		strcpy(ppResults[i]->CJSL, ppTable[i * COL_EACH_ROW + col_152]);
		//if (col_303 >= 0)
		strcpy(ppResults[i]->FSJE, ppTable[i * COL_EACH_ROW + col_303]);
		if (col_304 >= 0)
			strcpy(ppResults[i]->SYJE, ppTable[i * COL_EACH_ROW + col_304]);
		if (col_206 >= 0)
			strcpy(ppResults[i]->YJ, ppTable[i * COL_EACH_ROW + col_206]);
		if (col_210 >= 0)
			strcpy(ppResults[i]->YHS, ppTable[i * COL_EACH_ROW + col_210]);
		if (col_207 >= 0)
			strcpy(ppResults[i]->GHF, ppTable[i * COL_EACH_ROW + col_207]);
		if (col_208 >= 0)
			strcpy(ppResults[i]->CJF, ppTable[i * COL_EACH_ROW + col_208]);
		//if (col_155 >= 0)
		strcpy(ppResults[i]->CJBH, ppTable[i * COL_EACH_ROW + col_155]);
		if (col_167 >= 0)
			strcpy(ppResults[i]->CDBZ, ppTable[i * COL_EACH_ROW + col_167]);
		if (col_146 >= 0)
			strcpy(ppResults[i]->WTBH, ppTable[i * COL_EACH_ROW + col_146]);


		ppResults[i]->CJRQ_ = atoi(ppResults[i]->CJRQ);
		//ppResults[i]->CJSJ_ = atoi(ppResults[i]->CJSJ);
		ppResults[i]->MMBZ_ = atoi(ppResults[i]->MMBZ);
		ppResults[i]->WTLB_ = atoi(ppResults[i]->WTLB);
		ppResults[i]->CJJG_ = atof(ppResults[i]->CJJG);
		ppResults[i]->CJSL_ = atoi(ppResults[i]->CJSL);
		ppResults[i]->FSJE_ = atof(ppResults[i]->FSJE);
		ppResults[i]->SYJE_ = atof(ppResults[i]->SYJE);
		ppResults[i]->YJ_ = atof(ppResults[i]->YJ);
		ppResults[i]->YHS_ = atof(ppResults[i]->YHS);
		ppResults[i]->GHF_ = atof(ppResults[i]->GHF);
		ppResults[i]->CJF_ = atof(ppResults[i]->CJF);
		ppResults[i]->CDBZ_ = atoi(ppResults[i]->CDBZ);

		int HH = 0, mm = 0, ss = 0;
		GetUpdateTime_HH_mm_ss(ppResults[i]->CJSJ, &HH, &mm, &ss);
		ppResults[i]->CJSJ_ = HH * 10000 + mm * 100 + ss;
	}
}

void GetUpdateTime_HH_mm_ss(char* UpdateTime, int* _HH, int* _mm, int* _ss)
{
	*_HH = atoi(&UpdateTime[0]);
	*_mm = atoi(&UpdateTime[3]);
	*_ss = atoi(&UpdateTime[6]);
}

void DeleteStructs(void** ppStructs)
{
	if (ppStructs == nullptr)
		return;

	int i = 0;
	while (ppStructs[i] != 0)
	{
		delete[] ppStructs[i];

		++i;
	}

	delete[] ppStructs;
}

#else
#endif