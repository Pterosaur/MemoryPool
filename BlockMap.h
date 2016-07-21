#pragma once

//���������ͼ
//˵������¼�����ڴ��ʹ�����
#include "StaticList.h"

class BlockMap
{
public:
	typedef size_t Size;
	typedef size_t Address;
private:
	//��¼���ڿ��е�ַ
	gz::StaticList<Size> m_record;
public:
	BlockMap(Size size);
	~BlockMap();
	//���п����ڴ�
	bool HaveFreeMemory() const;
	//����һ����е�ַ
	Address GetFreeMemory();
	//�黹һ����ʹ�õ�ַ
	void GiveBackAMemory(Address address);
};

