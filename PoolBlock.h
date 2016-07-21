#pragma once
//类名:内存池数据块
#include "BlockMap.h"

//内存块存储对象的大小
template<size_t ObjectSize>
class PoolBlock
{
private:
	//内存块存储对象数量
	typedef size_t ObjectNumber;
	//存储数据块
	typedef char* DataBlock;
	//最大容量
	const ObjectNumber m_maxCapacity;
	//内存块地图
	BlockMap m_map;
	//数据块
	DataBlock m_data;
	//首地址
	inline size_t Address() const
	{
		return reinterpret_cast<size_t>(m_data);
	};
public:

	PoolBlock(ObjectNumber number = 1000) :
		m_maxCapacity(number)
		, m_data((char *)malloc(number * ObjectSize))
		, m_map(number)
	{
	};
	PoolBlock(const PoolBlock&& pb)
	{
		m_data = pb.m_data;
		const_cast<PoolBlock &>(pb).m_data = NULL;
	}
	PoolBlock(const PoolBlock&) = delete;
	PoolBlock& operator=(const PoolBlock&) = delete;
	~PoolBlock()
	{
		if (m_data)
		{
			free(m_data);
			m_data = NULL;
		}
	};
	//是否还有空闲空间
	bool HaveFreeMemory() const
	{
		return m_map.HaveFreeMemory();
	}
	//是否属于本内存快
	bool IsBelongingness(void *pointer) const
	{
		size_t address = reinterpret_cast<size_t>(pointer);
		return address >= Address() && address < Address() + m_maxCapacity * ObjectSize;
	}
	//申请一块内存
	void* Allocate()
	{
		BlockMap::Address relativeAddress = m_map.GetFreeMemory();
		if (-1 == relativeAddress)
			throw std::runtime_error("This block has be filled.");
		return reinterpret_cast<void *>(Address() + relativeAddress * ObjectSize);
	}
	//释放一块内存
	void Deallocate(void* pointer)
	{
		size_t address = reinterpret_cast<size_t>(pointer);
		BlockMap::Address relativeAddress = (address - Address())/ObjectSize;
		if (relativeAddress < 0 || relativeAddress >= m_maxCapacity)
			throw std::runtime_error("The pointer not belong to this block.");
		m_map.GiveBackAMemory(relativeAddress);
	}
};

