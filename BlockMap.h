#pragma once

//类名：块地图
//说明：记录块内内存的使用情况
#include "StaticList.h"

class BlockMap
{
public:
	typedef size_t Size;
	typedef size_t Address;
private:
	//记录块内空闲地址
	gz::StaticList<Size> m_record;
public:
	BlockMap(Size size);
	~BlockMap();
	//还有空闲内存
	bool HaveFreeMemory() const;
	//申请一块空闲地址
	Address GetFreeMemory();
	//归还一块已使用地址
	void GiveBackAMemory(Address address);
};

