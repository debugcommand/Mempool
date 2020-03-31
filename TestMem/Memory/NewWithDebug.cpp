#include "NewWithDebug.h"
#include <direct.h>

static map<const char*, ALLOCMEMORYINFO>* s_pAllocInfo = NULL;
map<const char*, ALLOCMEMORYINFO>& GetAllocInfo()
{
    if (s_pAllocInfo == NULL)
    {
        s_pAllocInfo = new map<const char*, ALLOCMEMORYINFO>;
    }
	return *s_pAllocInfo;

}
static map<void*, POINTERINFO>* s_pPointerInfo = NULL;

map<void*, POINTERINFO>& GetPointerInfo()
{
    if (s_pPointerInfo == NULL)
    {
        s_pPointerInfo = new map<void*, POINTERINFO>;
    }
	return *s_pPointerInfo;

}

static CSemiAutoIntLock* s_pnewcs = NULL;
CSemiAutoIntLock& GetAutoIntLock()
{
    if (s_pnewcs == NULL)
    {
        s_pnewcs = new CSemiAutoIntLock;
    }
	return *s_pnewcs;
}

extern void InitNewWithDebugContainer()
{
    GetAllocInfo();
    GetPointerInfo();
    GetAutoIntLock();
}

extern void FreeNewWithDebugContainer()
{
    if (s_pAllocInfo)
    {
        delete s_pAllocInfo;
        s_pAllocInfo = NULL;
    }
    if (s_pPointerInfo)
    {
        delete s_pPointerInfo;
        s_pPointerInfo = NULL;
    }
    if (s_pnewcs)
    {
        delete s_pnewcs;
        s_pnewcs = NULL;
    }
}

#include <fstream>
//将堆使用内存信息输出至文件
 void OutHeapMemoryInfoToFile(const char* szFileName)
 {
 	char szDate[256] = {0};
 	char szTime[256] = {0};
 	char szFile[256] = {0};
 
 	SYSTEMTIME systime;
 	GetLocalTime( &systime );
 
 	_snprintf(szDate, sizeof(szDate)-1, "%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
 	_snprintf(szTime, sizeof(szTime)-1, "%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
 
     char szPath[256] = {0};
     _snprintf(szPath, sizeof(szPath)-1, "Log/%s", szDate);
     _mkdir("Log");
     _mkdir(szPath);
 
     _snprintf(szFile, sizeof(szFile)-1, "%s/%s", szPath, szFileName);
 
     std::ofstream fLog(szFile, std::ios::app);
     if (!fLog.is_open())
     {
         return;
     }
 
     fLog << szDate << " " << szTime << std::endl;
     fLog << "---------------heapmemoryinfostart---------------" << std::endl;
 
     unsigned int nAllMemory = 0;
     for (map<const char*, ALLOCMEMORYINFO>::iterator itr = GetAllocInfo().begin();
         itr != GetAllocInfo().end(); itr++)
     {
         nAllMemory += itr->second.nAllUseMemory;
         fLog << (itr->first!=NULL?itr->first:"未定义用途") << " 占用内存[" << itr->second.nAllUseMemory << "]字节" << "  目前未释放共分配[" << itr->second.nExistCount << "]次  历史共申请[" << itr->second.nAllocCount << "]次" << std::endl;
     }
 
     fLog << "全部申请内存 " << nAllMemory << " Byte 共计 " << nAllMemory/(1024*1024) << " MByte" << std::endl;
     fLog << "---------------heapmemoryinfoend---------------" << std::endl;
 
 	fLog.close();
 };

#include <sstream>
 void OutHeapMemoryInfoToConsole()
 {
	 char szDate[256] = { 0 };
	 char szTime[256] = { 0 };
	 char szFile[256] = { 0 };
	 //char szLog[1024] = {0};

	 SYSTEMTIME systime;
	 GetLocalTime(&systime);

	 _snprintf(szDate, sizeof(szDate) - 1, "%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	 _snprintf(szTime, sizeof(szTime) - 1, "%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);

	 std::stringstream s;

	 s << szDate << " " << szTime << std::endl;
	 s << "---------------heapmemoryinfostart---------------" << std::endl;

	 unsigned int nAllMemory = 0;
	 for (map<const char*, ALLOCMEMORYINFO>::iterator itr = GetAllocInfo().begin();
		 itr != GetAllocInfo().end(); itr++)
	 {
		 nAllMemory += itr->second.nAllUseMemory;
		 s << (itr->first != NULL ? itr->first : "未定义用途") << " 占用内存[" << itr->second.nAllUseMemory << "]字节" << "  目前未释放共分配[" << itr->second.nExistCount << "]次  历史共申请[" << itr->second.nAllocCount << "]次" << std::endl;
	 }

	 s << "全部申请内存 " << nAllMemory << " Byte 共计 " << nAllMemory / (1024 * 1024) << " MByte" << std::endl;
	 s << "---------------heapmemoryinfoend---------------" << std::endl;
 };