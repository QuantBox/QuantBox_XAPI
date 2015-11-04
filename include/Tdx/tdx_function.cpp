#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#include "tdx_function.h"
#include "tdx_enum.h"
#include "tdx_field.h"

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

		// 最第一个的字符，并转成数字
		ppResults[i]->ZTSM_ = ppResults[i]->ZTSM[0] - '0';
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

void CharTable2GFLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GFLB_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	GFLB_STRUCT** ppResults = new GFLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_140 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQDM);
	int col_141 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQMC);
	int col_200 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQSL);
	int col_201 = GetIndexByFieldID(ppFieldInfos, FIELD_KMSL);
	int col_202 = GetIndexByFieldID(ppFieldInfos, FIELD_TBCBJ);
	int col_949 = GetIndexByFieldID(ppFieldInfos, FIELD_DQJ);
	int col_205 = GetIndexByFieldID(ppFieldInfos, FIELD_ZXSZ);
	int col_204 = GetIndexByFieldID(ppFieldInfos, FIELD_TBFDYK);
	int col_232 = GetIndexByFieldID(ppFieldInfos, FIELD_SXYK);
	int col_230 = GetIndexByFieldID(ppFieldInfos, FIELD_CKYKBL);
	int col_160 = GetIndexByFieldID(ppFieldInfos, FIELD_DJSL);
	int col_123 = GetIndexByFieldID(ppFieldInfos, FIELD_GDDM);
	int col_100 = GetIndexByFieldID(ppFieldInfos, FIELD_JYSDM);
	int col_101 = GetIndexByFieldID(ppFieldInfos, FIELD_JYSMC);
	int col_1213 = GetIndexByFieldID(ppFieldInfos, FIELD_BLXX);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GFLB_STRUCT();

		//if (col_140 >= 0)
			strcpy(ppResults[i]->ZQDM, ppTable[i * COL_EACH_ROW + col_140]);
		//if (col_141 >= 0)
			strcpy(ppResults[i]->ZQMC, ppTable[i * COL_EACH_ROW + col_141]);
		//if (col_200 >= 0)
			strcpy(ppResults[i]->ZQSL, ppTable[i * COL_EACH_ROW + col_200]);
		//if (col_201 >= 0)
			strcpy(ppResults[i]->KMSL, ppTable[i * COL_EACH_ROW + col_201]);
		//if (col_202 >= 0)
			strcpy(ppResults[i]->TBCBJ, ppTable[i * COL_EACH_ROW + col_202]);
		//if (col_949 >= 0)
			strcpy(ppResults[i]->DQJ, ppTable[i * COL_EACH_ROW + col_949]);
		//if (col_205 >= 0)
			strcpy(ppResults[i]->ZXSZ, ppTable[i * COL_EACH_ROW + col_205]);
		//if (col_204 >= 0)
			strcpy(ppResults[i]->TBFDYK, ppTable[i * COL_EACH_ROW + col_204]);
		//if (col_232 >= 0)
			strcpy(ppResults[i]->SXYK, ppTable[i * COL_EACH_ROW + col_232]);
		//if (col_230 >= 0)
			strcpy(ppResults[i]->CKYKBL, ppTable[i * COL_EACH_ROW + col_230]);
		//if (col_160 >= 0)
			strcpy(ppResults[i]->DJSL, ppTable[i * COL_EACH_ROW + col_160]);
		//if (col_123 >= 0)
			strcpy(ppResults[i]->GDDM, ppTable[i * COL_EACH_ROW + col_123]);
		//if (col_100 >= 0)
			strcpy(ppResults[i]->JYSDM, ppTable[i * COL_EACH_ROW + col_100]);
		//if (col_101 >= 0)
			strcpy(ppResults[i]->JYSMC, ppTable[i * COL_EACH_ROW + col_101]);
		//if (col_1213 >= 0)
			strcpy(ppResults[i]->BLXX, ppTable[i * COL_EACH_ROW + col_1213]);


		ppResults[i]->ZQSL_ = atoi(ppResults[i]->ZQSL);
		ppResults[i]->KMSL_ = atoi(ppResults[i]->KMSL);
		ppResults[i]->TBCBJ_ = atof(ppResults[i]->TBCBJ);
		ppResults[i]->DQJ_ = atof(ppResults[i]->DQJ);
		ppResults[i]->ZXSZ_ = atof(ppResults[i]->ZXSZ);
		ppResults[i]->DJSL_ = atof(ppResults[i]->DJSL);
	}
}

void CharTable2ZJYE(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, ZJYE_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	ZJYE_STRUCT** ppResults = new ZJYE_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_132 = GetIndexByFieldID(ppFieldInfos, FIELD_BZ);
	int col_300 = GetIndexByFieldID(ppFieldInfos, FIELD_ZJYE);
	int col_301 = GetIndexByFieldID(ppFieldInfos, FIELD_KYZJ);
	int col_310 = GetIndexByFieldID(ppFieldInfos, FIELD_ZZC_310);
	int col_302 = GetIndexByFieldID(ppFieldInfos, FIELD_KQZJ);
	int col_121 = GetIndexByFieldID(ppFieldInfos, FIELD_ZJZH);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new ZJYE_STRUCT();

		if (col_132 >= 0)
			strcpy(ppResults[i]->BZ, ppTable[i * COL_EACH_ROW + col_132]);
		if (col_300 >= 0)
			strcpy(ppResults[i]->ZJYE, ppTable[i * COL_EACH_ROW + col_300]);
		if (col_301 >= 0)
			strcpy(ppResults[i]->KYZJ, ppTable[i * COL_EACH_ROW + col_301]);
		if (col_310 >= 0)
			strcpy(ppResults[i]->ZZC, ppTable[i * COL_EACH_ROW + col_310]);
		if (col_302 >= 0)
			strcpy(ppResults[i]->KQZJ, ppTable[i * COL_EACH_ROW + col_302]);
		if (col_121 >= 0)
			strcpy(ppResults[i]->ZJZH, ppTable[i * COL_EACH_ROW + col_121]);


		ppResults[i]->ZJYE_ = atof(ppResults[i]->ZJYE);
		ppResults[i]->KYZJ_ = atof(ppResults[i]->KYZJ);
		ppResults[i]->ZZC_ = atof(ppResults[i]->ZZC);
		ppResults[i]->KQZJ_ = atof(ppResults[i]->KQZJ);
	}
}

void CharTable2HQ(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, HQ_STRUCT*** pppResults)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	HQ_STRUCT** ppResults = new HQ_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_140 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQDM);
	int col_141 = GetIndexByFieldID(ppFieldInfos, FIELD_ZQMC);
	int col_946 = GetIndexByFieldID(ppFieldInfos, FIELD_ZSJ);
	int col_945 = GetIndexByFieldID(ppFieldInfos, FIELD_JKJ);
	int col_948 = GetIndexByFieldID(ppFieldInfos, FIELD_GZLX);
	int col_949 = GetIndexByFieldID(ppFieldInfos, FIELD_DQJ);
	int col_910 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_PRICE_1);
	int col_911 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_PRICE_2);
	int col_912 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_PRICE_3);
	int col_913 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_PRICE_4);
	int col_914 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_PRICE_5);
	int col_900 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_SIZE_1);
	int col_901 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_SIZE_2);
	int col_902 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_SIZE_3);
	int col_903 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_SIZE_4);
	int col_904 = GetIndexByFieldID(ppFieldInfos, FIELD_BID_SIZE_5);
	int col_930 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_PRICE_1);
	int col_931 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_PRICE_2);
	int col_932 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_PRICE_3);
	int col_933 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_PRICE_4);
	int col_934 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_PRICE_5);
	int col_920 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_SIZE_1);
	int col_921 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_SIZE_2);
	int col_922 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_SIZE_3);
	int col_923 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_SIZE_4);
	int col_924 = GetIndexByFieldID(ppFieldInfos, FIELD_ASK_SIZE_5);
	int col_100 = GetIndexByFieldID(ppFieldInfos, FIELD_JYSDM);
	int col_187 = GetIndexByFieldID(ppFieldInfos, FIELD_ZXJYGS);
	int col_226 = GetIndexByFieldID(ppFieldInfos, FIELD_ZXMRBDJW);
	int col_227 = GetIndexByFieldID(ppFieldInfos, FIELD_ZXMCBDJW);
	int col_125 = GetIndexByFieldID(ppFieldInfos, FIELD_ZHLB);
	int col_132 = GetIndexByFieldID(ppFieldInfos, FIELD_BZ);
	int col_958 = GetIndexByFieldID(ppFieldInfos, FIELD_GZBS);
	int col_1213 = GetIndexByFieldID(ppFieldInfos, FIELD_BLXX);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new HQ_STRUCT();

		//if (col_140 >= 0)
			strcpy(ppResults[i]->ZQDM, ppTable[i * COL_EACH_ROW + col_140]);
		if (col_141 >= 0)
			strcpy(ppResults[i]->ZQMC, ppTable[i * COL_EACH_ROW + col_141]);
		if (col_946 >= 0)
			strcpy(ppResults[i]->ZSJ, ppTable[i * COL_EACH_ROW + col_946]);
		if (col_945 >= 0)
			strcpy(ppResults[i]->JKJ, ppTable[i * COL_EACH_ROW + col_945]);
		if (col_948 >= 0)
			strcpy(ppResults[i]->GZLX, ppTable[i * COL_EACH_ROW + col_948]);
		//if (col_949 >= 0)
			strcpy(ppResults[i]->DQJ, ppTable[i * COL_EACH_ROW + col_949]);
		//if (col_910 >= 0)
			strcpy(ppResults[i]->BidPrice1, ppTable[i * COL_EACH_ROW + col_910]);
		//if (col_911 >= 0)
			strcpy(ppResults[i]->BidPrice2, ppTable[i * COL_EACH_ROW + col_911]);
		//if (col_912 >= 0)
			strcpy(ppResults[i]->BidPrice3, ppTable[i * COL_EACH_ROW + col_912]);
		//if (col_913 >= 0)
			strcpy(ppResults[i]->BidPrice4, ppTable[i * COL_EACH_ROW + col_913]);
		//if (col_914 >= 0)
			strcpy(ppResults[i]->BidPrice5, ppTable[i * COL_EACH_ROW + col_914]);
		//if (col_900 >= 0)
			strcpy(ppResults[i]->BidSize1, ppTable[i * COL_EACH_ROW + col_900]);
		//if (col_901 >= 0)
			strcpy(ppResults[i]->BidSize2, ppTable[i * COL_EACH_ROW + col_901]);
		//if (col_902 >= 0)
			strcpy(ppResults[i]->BidSize3, ppTable[i * COL_EACH_ROW + col_902]);
		//if (col_903 >= 0)
			strcpy(ppResults[i]->BidSize4, ppTable[i * COL_EACH_ROW + col_903]);
		//if (col_904 >= 0)
			strcpy(ppResults[i]->BidSize5, ppTable[i * COL_EACH_ROW + col_904]);
		//if (col_930 >= 0)
			strcpy(ppResults[i]->AskPrice1, ppTable[i * COL_EACH_ROW + col_930]);
		//if (col_931 >= 0)
			strcpy(ppResults[i]->AskPrice2, ppTable[i * COL_EACH_ROW + col_931]);
		//if (col_932 >= 0)
			strcpy(ppResults[i]->AskPrice3, ppTable[i * COL_EACH_ROW + col_932]);
		//if (col_933 >= 0)
			strcpy(ppResults[i]->AskPrice4, ppTable[i * COL_EACH_ROW + col_933]);
		//if (col_934 >= 0)
			strcpy(ppResults[i]->AskPrice5, ppTable[i * COL_EACH_ROW + col_934]);
		//if (col_920 >= 0)
			strcpy(ppResults[i]->AskSize1, ppTable[i * COL_EACH_ROW + col_920]);
		//if (col_921 >= 0)
			strcpy(ppResults[i]->AskSize2, ppTable[i * COL_EACH_ROW + col_921]);
		//if (col_922 >= 0)
			strcpy(ppResults[i]->AskSize3, ppTable[i * COL_EACH_ROW + col_922]);
		//if (col_923 >= 0)
			strcpy(ppResults[i]->AskSize4, ppTable[i * COL_EACH_ROW + col_923]);
		//if (col_924 >= 0)
			strcpy(ppResults[i]->AskSize5, ppTable[i * COL_EACH_ROW + col_924]);
		//if (col_100 >= 0)
			strcpy(ppResults[i]->JYSDM, ppTable[i * COL_EACH_ROW + col_100]);
		//if (col_187 >= 0)
			strcpy(ppResults[i]->ZXJYGS, ppTable[i * COL_EACH_ROW + col_187]);
		//if (col_226 >= 0)
			strcpy(ppResults[i]->ZXMRBDJW, ppTable[i * COL_EACH_ROW + col_226]);
		//if (col_227 >= 0)
			strcpy(ppResults[i]->ZXMCBDJW, ppTable[i * COL_EACH_ROW + col_227]);
		//if (col_125 >= 0)
			strcpy(ppResults[i]->ZHLB, ppTable[i * COL_EACH_ROW + col_125]);
		//if (col_132 >= 0)
			strcpy(ppResults[i]->BZ, ppTable[i * COL_EACH_ROW + col_132]);
		//if (col_958 >= 0)
			strcpy(ppResults[i]->GZBS, ppTable[i * COL_EACH_ROW + col_958]);
		if (col_1213 >= 0)
			strcpy(ppResults[i]->BLXX, ppTable[i * COL_EACH_ROW + col_1213]);


		ppResults[i]->ZSJ_ = atof(ppResults[i]->ZSJ);
		ppResults[i]->JKJ_ = atof(ppResults[i]->JKJ);
		ppResults[i]->GZLX_ = atof(ppResults[i]->GZLX);
		ppResults[i]->DQJ_ = atof(ppResults[i]->DQJ);
		ppResults[i]->BidPrice1_ = atof(ppResults[i]->BidPrice1);
		ppResults[i]->BidPrice2_ = atof(ppResults[i]->BidPrice2);
		ppResults[i]->BidPrice3_ = atof(ppResults[i]->BidPrice3);
		ppResults[i]->BidPrice4_ = atof(ppResults[i]->BidPrice4);
		ppResults[i]->BidPrice5_ = atof(ppResults[i]->BidPrice5);
		ppResults[i]->AskPrice1_ = atof(ppResults[i]->AskPrice1);
		ppResults[i]->AskPrice2_ = atof(ppResults[i]->AskPrice2);
		ppResults[i]->AskPrice3_ = atof(ppResults[i]->AskPrice3);
		ppResults[i]->AskPrice4_ = atof(ppResults[i]->AskPrice4);
		ppResults[i]->AskPrice5_ = atof(ppResults[i]->AskPrice5);
		ppResults[i]->BidSize1_ = atoi(ppResults[i]->BidSize1);
		ppResults[i]->BidSize2_ = atoi(ppResults[i]->BidSize2);
		ppResults[i]->BidSize3_ = atoi(ppResults[i]->BidSize3);
		ppResults[i]->BidSize4_ = atoi(ppResults[i]->BidSize4);
		ppResults[i]->BidSize5_ = atoi(ppResults[i]->BidSize5);
		ppResults[i]->AskSize1_ = atoi(ppResults[i]->AskSize1);
		ppResults[i]->AskSize2_ = atoi(ppResults[i]->AskSize2);
		ppResults[i]->AskSize3_ = atoi(ppResults[i]->AskSize3);
		ppResults[i]->AskSize4_ = atoi(ppResults[i]->AskSize4);
		ppResults[i]->AskSize5_ = atoi(ppResults[i]->AskSize5);

		ppResults[i]->ZXJYGS_ = atoi(ppResults[i]->ZXJYGS);
		ppResults[i]->ZXMRBDJW_ = atof(ppResults[i]->ZXMRBDJW);
		ppResults[i]->ZXMCBDJW_ = atof(ppResults[i]->ZXMCBDJW);
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