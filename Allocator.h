#pragma once

#include "MemoryPool.h"

template <class T>
class Allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <class U>
	struct rebind
	{
		typedef Allocator<U> other;
	};


	Allocator()
	{
		return;
	}

	template <class U>
	Allocator(const Allocator<U>& c)
	{
	}
	//申请内存
	static pointer allocate(size_type n = 1)
	{
		if (n > 1)
			return m_stdAllocator.allocate(n);
		else
			return static_cast<pointer>(MemoryPool<sizeof(value_type)>::get_mutable_instance().Allocate());
	}
	//释放内存
	static void deallocate(pointer p, size_type n = 1)
	{
		if (n > 1)
			m_stdAllocator.deallocate(p, n);
		else
			MemoryPool<sizeof(value_type)>::get_mutable_instance().Deallocate(p);
	}
	//拷贝构造对象
	static void construct(pointer p, const value_type& other)
	{
		new(p)value_type(other);
	}
	//释放对象
	static void destroy(pointer p)
	{
		p->~value_type();
	}
	//创建一个新对象
	template<typename ... Args>
	static pointer New(Args & ... args)
	{
		pointer p = allocate();
		new(p)value_type(args...);
		return p;
	}
	//删除一个对象
	static void Delete(pointer p)
	{
		destroy(p);
		deallocate(p);
	}
	//得到地址
	static pointer address(reference x)
	{
		return (pointer)&x;
	}
	//得到地址
	static const_pointer const_address(const_reference x)
	{
		return (const_pointer)&x;
	}
	//最大申请量
	static size_type max_size()
	{
		return -1;
	}
private:
	//当前分配器无法满足时用STL自带分配器
	static std::allocator<value_type> m_stdAllocator;
};

template<typename value_type>
std::allocator<value_type> Allocator<value_type>::m_stdAllocator;