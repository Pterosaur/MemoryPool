#pragma once
//����:�ڴ�����ݿ�
#include "BlockMap.h"

//�ڴ��洢����Ĵ�С
template<size_t ObjectSize>
class PoolBlock
{
private:
	//�ڴ��洢��������
	typedef size_t ObjectNumber;
	//�洢���ݿ�
	typedef char* DataBlock;
	//�������
	const ObjectNumber m_maxCapacity;
	//�ڴ���ͼ
	BlockMap m_map;
	//���ݿ�
	DataBlock m_data;
	//�׵�ַ
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
	//�Ƿ��п��пռ�
	bool HaveFreeMemory() const
	{
		return m_map.HaveFreeMemory();
	}
	//�Ƿ����ڱ��ڴ��
	bool IsBelongingness(void *pointer) const
	{
		size_t address = reinterpret_cast<size_t>(pointer);
		return address >= Address() && address < Address() + m_maxCapacity * ObjectSize;
	}
	//����һ���ڴ�
	void* Allocate()
	{
		BlockMap::Address relativeAddress = m_map.GetFreeMemory();
		if (-1 == relativeAddress)
			throw std::runtime_error("This block has be filled.");
		return reinterpret_cast<void *>(Address() + relativeAddress * ObjectSize);
	}
	//�ͷ�һ���ڴ�
	void Deallocate(void* pointer)
	{
		size_t address = reinterpret_cast<size_t>(pointer);
		BlockMap::Address relativeAddress = (address - Address())/ObjectSize;
		if (relativeAddress < 0 || relativeAddress >= m_maxCapacity)
			throw std::runtime_error("The pointer not belong to this block.");
		m_map.GiveBackAMemory(relativeAddress);
	}
};

