#include "BlockMap.h"
#include "Accumulator.h"

BlockMap::BlockMap(Size size) : 
m_record(gz::Accumulator<Size>(), gz::Accumulator<Size>(size))
{
}

BlockMap::~BlockMap()
{
}

bool BlockMap::HaveFreeMemory() const
{
	return !m_record.empty();
}

BlockMap::Address BlockMap::GetFreeMemory()
{
	if (HaveFreeMemory())
	{
		BlockMap::Address freeMemoryAddress = *m_record.cbegin();
		m_record.pop_front();
		return freeMemoryAddress;
	}
	else
		return -1;
}

void BlockMap::GiveBackAMemory(Address address)
{
	//��Ϊ��m_record����Ե�ַ�Ǵ�1��ʼ������
	m_record.push_back(m_record.locatedByPosition(address + 1));
}
