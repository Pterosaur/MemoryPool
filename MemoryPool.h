#pragma once
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/serialization/singleton.hpp>
#include "PoolBlock.h"
#include "Singleton.h"
//ÄÚ´æ³Ø
template<size_t ObjectSize>
class MemoryPool
	:public boost::serialization::singleton<MemoryPool<ObjectSize> >
{
public:
	typedef PoolBlock<ObjectSize> Block;
	typedef std::shared_ptr<Block> BlockPtr;
private:
	//ÄÚ´æ¿é
	std::list<BlockPtr> m_pool;
	//»¥³âÐÅºÅÁ¿
	boost::mutex m_mutex;
	//Ìí¼ÓÄÚ´æ¿é
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
		//²éÕÒ¿ÕÏÐ¿é
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
