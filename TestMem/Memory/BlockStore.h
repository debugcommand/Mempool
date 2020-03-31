#ifndef __MENETWORKCORE_BLOCKSTORE_H__
#define __MENETWORKCORE_BLOCKSTORE_H__

#include <vector>
#include "NewWithDebug.h"

#ifdef _WIN32
//windows x86 or x68
#ifdef _WIN64 //x64
typedef uint64_t point_t;
#else //x86
typedef uint32_t point_t;
#endif //_WIN64
#else //unix
#ifdef __x86_64__ //x64
typedef unsigned __int64 point_t;
#elif __i386__ //x86
typedef unsigned int point_t;
#endif
#endif //_WIN32

namespace MPool
{
    class BlockStore
    {
    public:
         BlockStore(size_t blockSize, size_t blockPerBatch=100, size_t blockAlignment=4)
            : m_ppNextBlock(NULL), 
              m_blockSize((blockSize+blockAlignment-1)&(~(blockAlignment-1))), 
              m_blockPerBatch(blockPerBatch), m_blockAlignment(blockAlignment)
        {
    #ifdef _MT
            //�������ɶ��߳�,������
            InitializeCriticalSection( &m_cs );
    #endif
        }
        virtual ~BlockStore()
        {
            //����ڴ��������������ڴ�
            size_t iNum = m_batches.size();
            for (size_t i=0; i<iNum; ++i)
            {
                unsigned char* p = (unsigned char*)m_batches[i];
                CNewWithDebug<unsigned char>::Free( p );
            }

    #ifdef _MT        
                //�������ɶ��߳�,������
            DeleteCriticalSection( &m_cs );
    #endif
        }


    #ifdef _MT
            //�������ɶ��߳�,������
        void Lock() { EnterCriticalSection( &m_cs ); }
        void Unlock() { LeaveCriticalSection( &m_cs ); }
    #endif

        void* AllocateBlock()
        {  
    #ifdef _MT
            //�������ɶ��߳�,������
            Lock();
    #endif
            //��������ڴ�û�����¿���һ���ڴ�
            if (m_ppNextBlock == NULL || *m_ppNextBlock == NULL)
            {
                //�¿�һ���ڴ�(������ڴ��ԭ���ڴ��15�ֽ�,����������ֽڶ���)
                unsigned char* pNewBatch =  CNewWithDebug<unsigned char>::Alloc(m_blockPerBatch*m_blockSize+15, "BlockStore�����ڴ�");                
                _allocCount += m_blockPerBatch*m_blockSize+15;
                //��Ϊ�¿����ڴ���ԭ�����ڴ沢��������,�����Ҫ��vector������ɢ���ڴ��ָ��
                m_batches.push_back(pNewBatch);
                //��ʱ����������
                //#pragma warning(disable : 4311 4312)
                //16�ֽڶ���
                unsigned char* pAlignedPtr =(unsigned char*)((point_t)(pNewBatch+15)&(~15));
                //������Ԫ�ؿ��Է��õĵ�ַȷ��
                m_ppNextBlock = (unsigned char**)pAlignedPtr;
                for (size_t i=0; i<m_blockPerBatch-1; ++i)
                {
                    *((point_t*)(pAlignedPtr + i*m_blockSize)) = (point_t)(pAlignedPtr + (i+1)*m_blockSize);
                }
                *((point_t*)(pAlignedPtr + (m_blockPerBatch-1)*m_blockSize)) = (point_t)0;
            }

            unsigned char* pBlock = (unsigned char*)m_ppNextBlock;
            m_ppNextBlock = (unsigned char**)*m_ppNextBlock;
#ifdef _MT
            //�������ɶ��߳�,������
            Unlock();
#endif
            return (void*)pBlock;
        }

        void ReleaseBlock(void* pBlock)
        {
#ifdef _MT
            //�������ɶ��߳�,������
            Lock();
#endif
            if( pBlock )
            {
                //��ʱ����������
                //#pragma warning(disable : 4311)
                *((point_t*)pBlock) = (point_t)m_ppNextBlock;
                m_ppNextBlock = (unsigned char**)((unsigned char*)pBlock);
            }
#ifdef _MT
            //�������ɶ��߳�,������
            Unlock();
#endif
        }

        size_t GetAllocCount() { return _allocCount; }

    private:
        explicit BlockStore() : m_ppNextBlock(NULL), m_blockSize(1),  m_blockPerBatch(100), m_blockAlignment(4)
        {
    #ifdef _MT
            //�������ɶ��߳�,������
            InitializeCriticalSection( &m_cs );
    #endif              
        }

        explicit BlockStore(const BlockStore&)
            : m_ppNextBlock(NULL), 
              m_blockSize(1), 
              m_blockPerBatch(100),
              m_blockAlignment(4)
        {
    #ifdef _MT
            //�������ɶ��߳�,������
            InitializeCriticalSection( &m_cs );
    #endif
        }
        BlockStore& operator=(const BlockStore&){}

    public:

        typedef std::vector<unsigned char*> BatchPtrVector;

        unsigned char** m_ppNextBlock   ; // ָ���ڴ��Ԫ����һ��λ�õ�ָ��
        BatchPtrVector  m_batches       ; // ������ɢ���ڴ��ָ��
        const size_t    m_blockSize     ; // ÿ������ṹ���ڴ���ֽ���
        const size_t    m_blockPerBatch ; // ÿ��������ڴ������
        const size_t    m_blockAlignment; // �ڴ�����ֽ���
        size_t          _allocCount     ; // ��������

#ifdef _MT
        //�������ɶ��߳�,������
        CRITICAL_SECTION    m_cs;
#endif
    };
}

#endif // __MENETWORKCORE_BLOCKSTORE_H__