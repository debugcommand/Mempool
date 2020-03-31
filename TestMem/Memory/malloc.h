#ifndef _MALLOC_H_
#define _MALLOC_H_
#include "tcmalloc.h"
//基于tcmalloc的简单封装
template <typename T>
class G_Malloc
{
public:
    G_Malloc();
    virtual ~G_Malloc();

    static T* TcNew()
    {
        return new ((T*)tc_malloc(sizeof(T)))T();
    }

    static void TcDelete(T* pT)
    {
        if (pT)
        {
            pT->~T();
            tc_free(pT);
            pT = nullptr;
        }
    }

    static T* TcMalloc(unsigned int count = 1)
    {
        return (T*)tc_malloc(count*sizeof(T));
    }

    static void TcFree(T* pT) 
    {
        if (pT)
        {
            tc_free(pT);
            pT = nullptr;
        }
    }
};

#define  GNEW(TYPE)           G_Malloc< TYPE >::TcNew()
#define  GDEL(TYPE, POINTER)  G_Malloc< TYPE >::TcDelete(POINTER)
#define  GMALLOC(TYPE, COUNT)    G_Malloc< TYPE >::TcMalloc(COUNT)
#define  GFREE(TYPE, POINTER)  G_Malloc< TYPE >::TcFree(POINTER)
#endif
