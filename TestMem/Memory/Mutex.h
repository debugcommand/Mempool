#ifndef __MEUTILITY_MUTEX_H__
#define __MEUTILITY_MUTEX_H__
#pragma once

#ifdef _WIN32
#include <windows.h>
#include <MMsystem.h>
#pragma comment(lib,"winmm.lib")
#endif

class CCriticalSectionImp
{
public:
    virtual ~CCriticalSectionImp(){};

protected:
    CCriticalSectionImp()
    {}

protected:
#ifdef _WIN32
    CRITICAL_SECTION m_csAccess;
#else
    pthread_mutex_t m_csAccess;
#endif
};

class CSALocker;
class CSemiAutoIntLock : public CCriticalSectionImp
{
public:
    CSemiAutoIntLock() 
    {
    #ifdef _WIN32
        InitializeCriticalSection(&m_csAccess);
    #else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
        pthread_mutex_init(&m_csAccess,&attr);
        pthread_mutexattr_destroy(&attr);
    #endif
    }
    virtual ~CSemiAutoIntLock() 
    {
    #ifdef _WIN32
        DeleteCriticalSection(&m_csAccess);
    #else
        pthread_mutex_destroy(&m_csAccess);
    #endif
    }

protected:
    friend class CSALocker;
};

//半自动线程锁 (需要锁定先需要派生自 CSemiAutoIntLock, 然后使用 CSALocker 锁定m_csAccess;)
class CSALocker
{
public:
    explicit CSALocker(CSemiAutoIntLock* plock, DWORD dwTimeOut = 0, const char* szNote = NULL) : m_pcsAccess(NULL)
    {
        if (plock != NULL)
        {
#ifdef _WIN32
            DWORD dwStartTime = 0;

            if (dwTimeOut > 0 && szNote != NULL)
            {
                dwStartTime = timeGetTime();
            }
#endif
            m_pcsAccess = &plock->m_csAccess;
#ifdef _WIN32
            EnterCriticalSection(m_pcsAccess);
#else
            pthread_mutex_lock(m_pcsAccess);
#endif
#ifdef _WIN32
            if (dwTimeOut > 0 && szNote != NULL)
            {
                DWORD dwUseTime = timeGetTime() - dwStartTime;
                if (dwUseTime > dwTimeOut)
                {
                    printf("%s enterlock %dms overtime %d ms", szNote, dwUseTime, dwTimeOut);
                }
            }
#endif
        }
    }

    explicit CSALocker(CSemiAutoIntLock& lock, DWORD dwTimeOut = 0, const char* szNote = NULL) : m_pcsAccess(NULL)
    {
#ifdef _WIN32
        DWORD dwStartTime = 0;

        if (dwTimeOut > 0 && szNote != NULL)
        {
            dwStartTime = timeGetTime();
        }

#endif // 0
     
        m_pcsAccess = &lock.m_csAccess;
        
#ifdef _WIN32
        EnterCriticalSection(m_pcsAccess);
#else
        pthread_mutex_lock(m_pcsAccess);
#endif
#ifdef _WIN32
        if (dwTimeOut > 0 && szNote != NULL)
        {
            DWORD dwUseTime = timeGetTime() - dwStartTime;
            if (dwUseTime > dwTimeOut)
            {
                printf("%s enterlock %dms overtime %d ms", szNote, dwUseTime, dwTimeOut);
            }
        }
#endif
    }

    virtual ~CSALocker()
    {
        if (m_pcsAccess != NULL)
        {
#ifdef _WIN32
            LeaveCriticalSection(m_pcsAccess);
#else
            pthread_mutex_unlock(m_pcsAccess);
#endif
        }
    }

private:
#ifdef _WIN32
    LPCRITICAL_SECTION m_pcsAccess;
#else
    pthread_mutex_t* m_pcsAccess;
#endif
private:
    CSALocker();
    CSALocker(const CSALocker&);
    CSALocker& operator = (const CSALocker&);
};

class CIntLock : public CCriticalSectionImp
{
public:    
    CIntLock()
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_csAccess);
#else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
        pthread_mutex_init(&m_csAccess,&attr);
        pthread_mutexattr_destroy(&attr);
#endif
    }
    virtual ~CIntLock()
    {
#ifdef _WIN32
        DeleteCriticalSection(&m_csAccess);
#else
        pthread_mutex_destroy(&m_csAccess);
#endif    
    }

public:
    void Lock()
    {
#ifdef _WIN32
        EnterCriticalSection(&m_csAccess);
#else
        pthread_mutex_lock(&m_csAccess);
#endif
    }
    
    void Unlock()
    {
#ifdef _WIN32
        LeaveCriticalSection(&m_csAccess);
#else
        pthread_mutex_unlock(&m_csAccess);
#endif
    }

private:
    CIntLock(const CIntLock&)
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_csAccess);
#else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
        pthread_mutex_init(&m_csAccess,&attr);
        pthread_mutexattr_destroy(&attr);
#endif
    }
    CIntLock operator = (const CIntLock&) { return *this; }
};

#endif // __MEUTILITY_MUTEX_H__
