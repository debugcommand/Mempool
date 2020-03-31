/*
Abstract:
	提供一个简单的线程安全的内存池模板
	其中线程安全部分需要使用WIN32 API

	警告: TObject_pool只为类设计使用,所以并不需要连续内存块
	所以严禁一次性new出数组,像这样是被严格禁止的:

	class TestObject : public NCCommon::TObject_pool<TestObject>
	new TestObject [xxx]

	正确使用方式:
	for (int i=0; i<xxx; ++i)
	{
		new TestObject;
	}

	而TAllocator则允许一次性分配连续的内存块：
	NCCommon::TAllocator<TestObject> Allocator;
	TestObject* p = Allocator(100);
	等同于:
	TestObject* p = new TestObject[100];
*/

#ifndef __MENETWORKCORE_MEMORY_POOL_H__
#define __MENETWORKCORE_MEMORY_POOL_H__

#pragma once
#include <memory>
#include "BlockStore.h"
#include "NewWithDebug.h"
#include "Mutex.h"
#include <assert.h>

namespace MPool
{
/*++
参数:
	blocksPerBatch: 每次申请的内存块大小
	blockAlignment: 机器CPU字节长度
--*/
    template <typename T, size_t blocksPerBatch=100, size_t blockAlignment=4>
    class TObject_pool
    {
    public:
        __inline void* operator new(size_t)
        {
            return s_Store.AllocateBlock();
        }

		__inline void* operator new(size_t,int, const char *, int)
		{
			return s_Store.AllocateBlock();
		}

        __inline void operator delete(void* pBlock)
        {
            s_Store.ReleaseBlock((T*)pBlock);
        }
        static size_t GetAllocCount() { return s_Store.GetAllocCount(); }
    // 将构造函数设置成protected,使之成为一纯虚基类模板
	protected:
        TObject_pool(){}

    private:
        static BlockStore s_Store;
    };

    template<class T, size_t blocksPerBatch, size_t blockAlignment> 
    typename MPool::BlockStore TObject_pool<T, blocksPerBatch, blockAlignment>::s_Store(sizeof(T), blocksPerBatch, blockAlignment);
};

#define INHERIT_POOL( className ) \
    public MPool::TObject_pool<className>

#define INHERIT_POOL_PARAM( className, count ) \
    public MPool::TObject_pool<className, count>

#define DECLARE_POOL_FUNC( className ) \
    static  className* CreateInstance() { return MES_NEW(className); } \
    virtual void       Release()        { MES_DELETE(className);     }

#define NEW_POOL( className ) \
    className::CreateInstance();

#endif // __MENETWORKCORE_MEMORY_POOL_H__ 
