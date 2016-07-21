#pragma once
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/serialization/singleton.hpp>
#include "PoolBlock.h"
#include "Singleton.h"
//�ڴ��
template<size_t ObjectSize>
class MemoryPool
	:public boost::serialization::singleton<MemoryPool<ObjectSize> >
{
public:
	typedef PoolBlock<ObjectSize> Block;
	typedef std::shared_ptr<Block> BlockPtr;
private:
	//�ڴ��
	std::list<BlockPtr> m_pool;
	//�����ź���
	boost::mutex m_mutex;
	//����ڴ��
	BlockPtr AddBlock()
	{
		BlockPtr newBlock(new Block());
		m_pool.push_front(newBlock);
		return newBlock;
	}
public:
	MemoryPool()
	{
	};
	~MemoryPool()
	{
	};
	void* Allocate()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		//���ҿ��п�
		auto freeBlock = std::find_if(m_pool.cbegin(), m_pool.cend(), [](const BlockPtr& bp){return bp->HaveFreeMemory(); });
		if (freeBlock == m_pool.cend())
			return AddBlock()->Allocate();
		else
			return (*freeBlock)->Allocate();
	};
	void Deallocate(void* pointer)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		auto block = std::find_if(m_pool.crbegin(), m_pool.crend(), [=](const BlockPtr& bp){return bp->IsBelongingness(pointer); });
		auto end = m_pool.crend();
		(*block)->Deallocate(pointer);
	}
};
