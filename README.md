# 简介
一个简单的线程安全的内存池模板
其中线程安全部分需要使用WIN32 API
支持x86,x64
警告: TObject_pool只为类设计使用,所以并不需要连续内存块
所以严禁一次性new出数组,像这样是被严格禁止的:

class TestObject : public NCCommon::TObject_pool<TestObject>
new TestObject [xxx]

正确使用方式:
for (int i=0; i<xxx; ++i)
{
	new TestObject;
}
与new以及tcmalloc做一个简单测试,不尽人意...尝试修改
/////////////////////////////////////////////////////
编译测试IDE：vs2017
linux下暂未测试

//将堆使用内存信息输出至文件
void OutHeapMemoryInfoToFile(const char* szFileName);
void OutHeapMemoryInfoToConsole();

tcmalloc库:https://github.com/gperftools/gperftools
