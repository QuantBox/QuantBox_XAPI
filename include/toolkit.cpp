#include "stdafx.h"
#include "toolkit.h"
#include <string.h>

#if defined _WIN32 || WIN32 || _WINDOWS
    #include <direct.h>
    #define MKDIR(X) _mkdir((X));
#else
    #include <sys/stat.h>
    #define MKDIR(X) mkdir((X),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif // defined


void makedirs(const char* dir)
{
	if (nullptr == dir)
		return;

	int len = strlen(dir);

	char * p = new char[len+1];
	strcpy(p,dir);
	for (int i=0;i<len;++i)
	{
		char ch = p[i];
		if ('\\' == ch ||'/' == ch)
		{
			p[i] = '\0';
			MKDIR(p);
			p[i] = ch;
		}
	}
	delete[] p;
}

char* GetSetFromString(const char* szString, const char* seps, vector<char*>& vct, set<char*>& st, int modify, set<string>& st2)
{
	vct.clear();
	st.clear();

	if(nullptr == szString
		||nullptr == seps)
		return nullptr;

	if(strlen(szString)==0
		||strlen(seps)==0)
		return nullptr;

	size_t len = strlen(szString)+1;
	char* buf = new char[len];
	strncpy(buf,szString,len);

	char* token = strtok(buf, seps);
	while(token)
	{
		if (strlen(token)>0)
		{
			if (modify > 0)
				st2.insert(token);
			else if (modify<0)
				st2.erase(token);

			vct.push_back(token);
			st.insert(token);
		}
		token = strtok( nullptr, seps);
	}

	return buf;
}

void GetOnFrontDisconnectedMsg(int ErrorId, char* ErrorMsg)
{
	switch (ErrorId)
	{
	case 0x1001:
		strcpy(ErrorMsg,"0x1001 4097 ÍøÂç¶ÁÊ§°Ü");
		break;
	case 0x1002:
		strcpy(ErrorMsg,"0x1002 4098 ÍøÂçÐ´Ê§°Ü");
		break;
	case 0x2001:
		strcpy(ErrorMsg,"0x2001 8193 ½ÓÊÕÐÄÌø³¬Ê±");
		break;
	case 0x2002:
		strcpy(ErrorMsg,"0x2002 8194 ·¢ËÍÐÄÌøÊ§°Ü");
		break;
	case 0x2003:
		strcpy(ErrorMsg,"0x2003 8195 ÊÕµ½´íÎó±¨ÎÄ");
		break;
	default:
		sprintf(ErrorMsg, "%x %d Î´Öª´íÎó", ErrorId, ErrorId);
		break;
	}
}
