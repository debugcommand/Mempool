/*
Abstract:
	�ṩһ���򵥵��̰߳�ȫ���ڴ��ģ��
	�����̰߳�ȫ������Ҫʹ��WIN32 API

	����: TObject_poolֻΪ�����ʹ��,���Բ�����Ҫ�����ڴ��
	�����Ͻ�һ����new������,�������Ǳ��ϸ��ֹ��:

	class TestObject : public NCCommon::TObject_pool<TestObject>
	new TestObject [xxx]

	��ȷʹ�÷�ʽ:
	for (int i=0; i<xxx; ++i)
	{
		new TestObject;
	}

	��TAllocator������һ���Է����������ڴ�飺
	NCCommon::TAllocator<TestObject> Allocator;
	TestObject* p = Allocator(100);
	��ͬ��:
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
����:
	blocksPerBatch: ÿ��������ڴ���С
	blockAlignment: ����CPU�ֽڳ���
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
    // �����캯�����ó�protected,ʹ֮��Ϊһ�������ģ��
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
