#pragma once
#include <map>
#include "Mutex.h"
using std::map;
using std::pair;

//ÿ����;���ڴ�ʹ����Ϣ
typedef struct AllocMemoryInfo
{
    unsigned int nExistCount;   // ��ǰδ�ͷŵ��������
    unsigned int nAllocCount;   // ����;���ڴ湲�����˶��ٴ�
    unsigned int nAllUseMemory; // ����;���ڴ���ռ�õ��ڴ��С
    unsigned int nAllUseMemoryMax; 
}ALLOCMEMORYINFO;

typedef struct PointerInfo
{
    unsigned int nSize;
    unsigned int nAllocCount; // ��ָ�������������͵ĸ����������ж�ɾ��ʱ���� delete p ���� delete [] p��
    const char*  szNote;      // ��ָ��������;
}POINTERINFO;

extern std::map<const char*, ALLOCMEMORYINFO>& GetAllocInfo();
extern std::map<void*, POINTERINFO>& GetPointerInfo();
extern CSemiAutoIntLock& GetAutoIntLock();

extern void InitNewWithDebugContainer();
extern void FreeNewWithDebugContainer();
    
//ʹ���߱���ʹ��CNewWithDebug<typename>::Alloc
template <typename T>
class CNewWithDebug
{
public:
	CNewWithDebug();
    virtual ~CNewWithDebug();

    static T* Alloc (unsigned int nCount = 1, const char* szNote = NULL)
    {
        //��׼c++�����new���飬��ʹ����Ԫ��Ϊ0��Ҳ������Ϊ1
        //new int[0] ��ͬ�� new int[1];
        if (nCount == 0)
        {
            nCount = 1;
        }

        T* p = NULL;
        if (nCount > 1)
        {
            p = new T[nCount];
        }
        else
        {
            p = new T;
        }

        POINTERINFO pinfo = {0};
        pinfo.nAllocCount = nCount;
        pinfo.nSize = sizeof(T)*nCount;
        pinfo.szNote = szNote;


		CSALocker locker(GetAutoIntLock());

        GetPointerInfo()[p] = pinfo;

        std::map<const char*, ALLOCMEMORYINFO>::iterator itr = GetAllocInfo().find(szNote);

        if (itr == GetAllocInfo().end())
        {
            ALLOCMEMORYINFO ainfo = {0};
            ainfo.nAllocCount = 1;
            ainfo.nExistCount = 1;
            ainfo.nAllUseMemory += pinfo.nSize;
            GetAllocInfo().insert(std::pair<const char*, ALLOCMEMORYINFO>(szNote, ainfo));

            ainfo.nAllUseMemoryMax = ainfo.nAllUseMemory;
        }
        else
        {
            itr->second.nAllocCount ++;
            itr->second.nExistCount ++;
            itr->second.nAllUseMemory += pinfo.nSize;

            if ( itr->second.nAllUseMemoryMax < itr->second.nAllUseMemory )
            { itr->second.nAllUseMemoryMax = itr->second.nAllUseMemory; }
        }

        return p;
    }

    static void Free(T* p)
    {
        unsigned int nCount;

        {
            CSALocker locker(GetAutoIntLock());

            if (p == NULL)
            {
                return;
            }
            std::map<void*, POINTERINFO>::iterator itr = GetPointerInfo().find(p);
            if (itr != GetPointerInfo().end())
            {
                std::map<const char*, ALLOCMEMORYINFO>::iterator allinfoitr = GetAllocInfo().find(itr->second.szNote);
                if (allinfoitr != GetAllocInfo().end())
                {
                    allinfoitr->second.nExistCount --;
                    allinfoitr->second.nAllUseMemory -= itr->second.nSize;
                }
                nCount = itr->second.nAllocCount;
                GetPointerInfo().erase(itr);
            }
            else
            {
                return;
            }

		}

        if (nCount >1)
        {
            delete [] p;
        }
        else
        {
            delete p;
        }
    }
};

#define MES_NEW(className)                      CNewWithDebug<className>::Alloc(1,    "ʵ��"#className)
#define MES_NEW_SIZE(className,size )           CNewWithDebug<className>::Alloc(size, "ʵ��"#className)
#define MES_NEW_SIZE_NAME(className,size,name ) CNewWithDebug<className>::Alloc(size, name );
#define MES_DELETE(className)                   CNewWithDebug<className>::Free(this);
#define MES_DELETE_P(className, pointer)        CNewWithDebug<className>::Free(pointer);

//����ʹ���ڴ���Ϣ������ļ�
void OutHeapMemoryInfoToFile(const char* szFileName);
void OutHeapMemoryInfoToConsole();