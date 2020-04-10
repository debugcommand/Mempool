// TtestMem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <iostream>
#include "Memory/malloc.h"
#include <psapi.h>
#include "Memory/Memory_Pool.h"
#include <MMSystem.h>
#include "Memory/circularbuffer.h"

#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"winmm.lib")
using namespace std;
#define MYC_COUNT 100000
class Myclass:INHERIT_POOL_PARAM(Myclass, MYC_COUNT)
{
public:
    Myclass() {
        pCh = new char[4];
        n = 10;
        m = 11;
    }
    virtual ~Myclass() {
        //cout << "~Myclass" << endl;
        delete[]pCh;
        n = 0;
        m = 0;
    }
    DECLARE_POOL_FUNC(Myclass);
    char* pCh;
    int n;
    int m;
};

class Myclass2// :INHERIT_POOL_PARAM(Myclass2, 100)
{
public:
    Myclass2() {
        pCh = new char[4];
        n = 10;
        m = 11;
    }
    virtual ~Myclass2() {
        //cout << "~Myclass2" << endl;
        delete[]pCh;
        n = 0;
        m = 0;
    }
    //DECLARE_POOL_FUNC(Myclass2);

    void Inc(int num) {
        n = num;
        m = num;
    }

    char* pCh;
    int n;
    int m;
};
void showMemoryInfo(void)
{
    HANDLE handle = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
    //
    cout << "内存使用:" << pmc.WorkingSetSize / 1000 << "K/" << pmc.PeakWorkingSetSize / 1000 << "K + " << pmc.PagefileUsage / 1000 << "K/" << pmc.PeakPagefileUsage / 1000 << "K" << endl;
}

int main()
{
#if 1
    showMemoryInfo();
    EmptyWorkingSet(GetCurrentProcess());
    showMemoryInfo();
    //////////////////////////////////////
    Myclass* pMyClassList[MYC_COUNT];
    Myclass2* pMyClassList2[MYC_COUNT];
    DWORD curT = timeGetTime();
    DWORD startT = curT;
    cout << "pool new:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        pMyClassList[i] = Myclass::CreateInstance();
    }
    curT = timeGetTime();
    cout << "pool new cost:" << curT - startT << endl;
    startT = timeGetTime();
    cout << "pool release:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        pMyClassList[i]->Release();
    }
    curT = timeGetTime();
    cout << "pool release cost:" << curT - startT << endl;
    //
    startT = timeGetTime();
    cout << "new:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        pMyClassList2[i] = new Myclass2();
    }
    curT = timeGetTime();
    cout << "new cost:" << curT - startT << endl;
    startT = timeGetTime();
    cout << "delete:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        delete pMyClassList2[i];
    }
    curT = timeGetTime();
    cout << "delete cost:" << curT - startT << endl;
    //
    startT = timeGetTime();
    cout << "tc new:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        pMyClassList2[i] = GNEW(Myclass2);
    }
    curT = timeGetTime();
    cout << "tc new cost:" << curT - startT << endl;
    startT = timeGetTime();
    cout << "tc delete:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT; i++)
    {
        GDEL(Myclass2, pMyClassList2[i]);
    }
    curT = timeGetTime();
    cout << "tc delete cost:" << curT - startT << endl;
    //
    int class2_size = sizeof(Myclass2);
    Myclass2 mc2;
    CircularBuffer circularbuf_(MYC_COUNT*class2_size+1);
    startT = timeGetTime();
    cout << "write buffer:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT+1; i++)
    {
        mc2.Inc(i);
        if (circularbuf_.write((char*)&mc2, class2_size) <= 0) {
            printf("write error!\n");
        }
    }
    curT = timeGetTime();
    cout << "write buffer cost:" << curT - startT << endl;
    Myclass2* pMc2 = new Myclass2();
    startT = timeGetTime();
    cout << "read buffer:" << startT << endl;
    for (size_t i = 0; i < MYC_COUNT+1; i++)
    {
        if (circularbuf_.read((char*)pMc2, class2_size) <= 0) {
            printf("read error!\n");
        }
    }
    curT = timeGetTime();
    cout << "read buffer cost:" << curT - startT << endl;    
    //////////////////////////////////////////////////
    while (1)
    {
        Sleep(100);
    }
    return 0;
#endif // 0    
}